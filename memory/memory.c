#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "../util/util.h"
#include "../include/constants.h"
#include "../include/io.h"
#include "../process/process.h"
#include "../arm/critical_section.h"

#define MAP_ENTRY_LEN 64

/* data */
void *start_address;
static MemoryManager *mmr = NULL;


/* public functions */

void initialize_memory(void) {
    void *system_memory_address;

    #ifdef SYSTEM_MEMORY_ADDRESS /* on the K70 */
    system_memory_address = (void *) SYSTEM_MEMORY_ADDRESS;
    #else
    /* obtain chunk of memory from system for myMalloc and myFree */
    system_memory_address = malloc(TOTAL_SPACE);
    #endif
    if (system_memory_address == NULL) {
        efputs("initialize_shell: could not allocate system memory\r\n", STDERR);
        return;
    }
    configure_memory(system_memory_address, TOTAL_SPACE);
}


/*
 * configure_memory
 * Purpose:
 *  setup memory manager
 *  client must call this prior to calling myFree, myMalloc, and memoryMap
 *
 * Parameters:
 *  start_address - the starting address of the memory span
 *      from which myMalloc will allocate
 *  total_space - the total amount of space used by this manager manager,
 *      including both bookkeeping info and the memory allocated to callers
 *
 * Returns:
 *  Pointer to the initialized MemoryManager struct
 *
 * Side-Effects:
 *  None
 */
MemoryManager *configure_memory(void *start_address,
                                unsigned int total_space) {
    MemoryManager *mmr;
    uintptr_t gap;

    set_start_address(start_address);
    mmr = (MemoryManager *) start_address;
    /* assuming that start_address is at a double word boundary */
    /* ensure that the bookkeeping space at the start */
    /* is double word aligned */
    /* then we will add the initial double word Region */
    /* so the first returned address will be double word aligned */
    gap = double_word_align(sizeof(MemoryManager));
    mmr->start_of_memory = ((uintptr_t) start_address + gap);
    mmr->end_of_memory = ((uintptr_t) start_address + total_space);
    mmr->remaining_space = total_space - gap;
    mmr->base_region = create_base_region(mmr);
    return mmr;
}


/*
 * myMalloc
 * Purpose:
 *  allocate memory and return its address
 *
 * Parameters:
 *  size - the number of bytes desired
 *
 * Returns:
 *  the address of the allocated memory on success, 0 otherwise
 *
 * Side-Effects:
 *  Allocates heap storage
 */
void *myMalloc(unsigned int size) {
    Region *r;
    disable_interrupts();
    if (mmr == NULL) {
        mmr = configure_memory(start_address, TOTAL_SPACE);
    }

    if (size == 0) {
        enable_interrupts();
        return NULL;
    }

    size = double_word_align(size);
    r = next_free_region_of_size(mmr, size);
    if (r == 0) {
        enable_interrupts();
        return NULL;
    }
    allocate_region(mmr, r, size);
    enable_interrupts();
    return r->data;
}


/*
 * myFree
 * Purpose:
 *  deallocate the memory pointed to by the given address
 *
 * Parameters:
 *  ptr - a memory address previously allocated by myMalloc
 *
 * Returns:
 *  None
 *
 * Side-Effects:
 *  deallocates heap storage
 *  consolidates free memory regions
 */
void myFree(void *ptr) {
    Region *r;
    disable_interrupts();
    if (mmr == NULL) {
        mmr = configure_memory(start_address, TOTAL_SPACE);
    }

    if (ptr == 0) {
        enable_interrupts();
        return;
    }

    if (!is_valid_pointer(mmr, ptr)) {
        enable_interrupts();
        return;
    }
    r = region_for_pointer(ptr);
    r->free = 1;
    increase_remaining_space(mmr, r->size);

    if (can_merge_next(mmr, r)) {
        merge_next(mmr, r);
    }
    if (can_merge_previous(mmr, r)) {
        merge_previous(mmr, r);
    }
    enable_interrupts();
}


/*
 * memoryMap
 * Purpose:
 *  output the map of both allocated and free memory regions
 *
 * Parameters:
 *  None
 *
 * Returns:
 *  None
 *
 * Side-Effects:
 *  None
 */
void memoryMap(void) {
    MemoryManager *mmr;
    Region *current, *final;
    disable_interrupts();
    mmr = (MemoryManager *) start_address;
    current = mmr->base_region;
    final = final_region(mmr);
    char map_entry[MAP_ENTRY_LEN];
    char *status[] = { "used", "free"}; /* 0 == used, 1 == free */
    while (TRUE) {
        sprintf(map_entry, "%p: PID %d %d bytes, %s\r\n",
            current->data,
            (int) current->pid,
            current->size,
            status[current->free]);
        efputs(map_entry, STDOUT);
        if (current == final) {
            break;
        }
        current = next_region(current);
    }
    enable_interrupts();
}


/* internal helper functions */
void *test_myMalloc(MemoryManager *test_mmr, unsigned int size) {
    mmr = test_mmr;
    return (myMalloc(size));
}

void test_myFree(MemoryManager *test_mmr, void *ptr) {
    mmr = test_mmr;
    myFree(ptr);
}

void set_start_address(void *addr) {
    start_address = addr;
}

Region *create_base_region(MemoryManager *mmr) {
    Region *r = (Region *) mmr->start_of_memory;
    r->free = 1;
    decrease_remaining_space(mmr, sizeof(Region));
    r->size = mmr->remaining_space;
    return r;
}

unsigned int double_word_align(unsigned int size) {
    unsigned int double_word_size = 2 * WORD_SIZE;
    unsigned int padding = double_word_size - 1;
    return (size + padding) & ~padding;
}

void allocate_region(MemoryManager *mmr, Region *r, unsigned int size) {
    Region *end_of_current = (Region *) (r->data + (uintptr_t) size);
    unsigned int space_at_end = r->size - size;
    if (space_at_end >= sizeof(Region)) {
        append_region(mmr, end_of_current, space_at_end);
    } else {
        size = size + space_at_end;
    }
    r->size = size;
    r->free = 0;
    r->pid = getpid();
    decrease_remaining_space(mmr, size);
}

void append_region(MemoryManager *mmr, Region *end, unsigned int size) {
    end->free = 1;
    end->size = size - sizeof(Region);
    decrease_remaining_space(mmr, sizeof(Region));
}


Region *next_region(Region *current) {
        uintptr_t shift;
        shift = ((uintptr_t) current + sizeof(Region) + current->size);
        return ((Region *) shift);
}

Region *next_free_region_of_size(MemoryManager *mmr, unsigned int size) {
    Region *cursor = mmr->base_region;
    Region *final = final_region(mmr);
    while (TRUE) {
        if (cursor->free && cursor->size >= size) {
            return cursor;
        }
        if (cursor == final) {
            return 0;
        }
        cursor = next_region(cursor);
    }
}

Region *previous_region(MemoryManager *mmr, Region *current) {
    Region *prev, *next, *final;
    prev = mmr->base_region;
    final = final_region(mmr);
    while (prev != current && prev != final) {
        next = next_region(prev);
        if (next == current) {
            return prev;
        }
        prev = next;
    }
    return (Region *) 0;
}

Region *final_region(MemoryManager *mmr) {
    Region *cursor = mmr->base_region;
    uintptr_t address = (uintptr_t) cursor->data + (uintptr_t) cursor->size;
    while (address < mmr->end_of_memory) {
        cursor = next_region(cursor);
        address = (uintptr_t) cursor->data + (uintptr_t) cursor->size;
    }
    return cursor;
}

unsigned int remaining_space(MemoryManager *mmr) {
    return mmr->remaining_space;
}

void decrease_remaining_space(MemoryManager *mmr, unsigned int size) {
    mmr->remaining_space -= size;
}

void increase_remaining_space(MemoryManager *mmr, unsigned int size) {
    mmr->remaining_space += size;
}


Region *region_for_pointer(void *ptr) {
    return ((Region *) ptr - 1);
}

int is_valid_pointer(MemoryManager *mmr, void *ptr) {
    Region *cursor = mmr->base_region;
    Region *final = final_region(mmr);
    while (TRUE) {
        if (ptr == cursor->data) {
            return TRUE;
        }
        if (cursor == final) {
            return FALSE;
        }
        cursor = next_region(cursor);
    }
}

void merge_next(MemoryManager *mmr, Region *r) {
    Region *next = next_region(r);
    unsigned int merged_space = next->size + sizeof(Region);
    r->size = r->size + merged_space;
    increase_remaining_space(mmr, sizeof(Region));
}

void merge_previous(MemoryManager *mmr, Region *r) {
    Region *prev = previous_region(mmr, r);
    unsigned int merged_space = r->size + sizeof(Region);
    prev->size = prev->size + merged_space;
    increase_remaining_space(mmr, sizeof(Region));
}

int can_merge_next(MemoryManager *mmr, Region *r) {
    if (r == final_region(mmr)) {
        return FALSE;
    }
    if (next_region(r)->free) {
        return TRUE;
    }
    return FALSE;
}

int can_merge_previous(MemoryManager *mmr, Region *r) {
    Region *prev = previous_region(mmr, r);
    if (prev == 0) {
        return FALSE;
    }
    if (prev->free) {
        return TRUE;
    }
    return FALSE;
}
