#include <stdint.h>
#include <stdio.h>
#include "memory.h"
#include "../util/util.h"
#include "../include/constants.h"


void *start_address;
static MemoryManager *mmr = 0;

uint32_t getCurrentPID(void) {
    return 0;
}

void set_start_address(void *addr) {
    start_address = addr;
}

MemoryManager *initialize_memory(void *start_address,
                                unsigned int total_space) {
    set_start_address(start_address);
    MemoryManager *mmr = (MemoryManager *) start_address;
    /* assuming that start_address is at a double word boundary */
    /* ensure that the bookkeeping space at the start */
    /* is double word aligned */
    /* then we will add the initial double word Region */
    /* so the first returned address will be double word aligned */
    uintptr_t gap = double_word_align(sizeof(MemoryManager));
    mmr->start_of_memory = ((uintptr_t) start_address + gap);
    mmr->end_of_memory = ((uintptr_t) start_address + total_space);
    mmr->remaining_space = total_space - gap;
    mmr->base_region = create_base_region(mmr);
    return mmr;
}

void *test_myMalloc(MemoryManager *test_mmr, unsigned int size) {
    mmr = test_mmr;
    return (myMalloc(size));
}

void *myMalloc(unsigned int size) {
    Region *r;
    if (mmr == 0) {
        mmr = initialize_memory(start_address, TOTAL_SPACE);
    }

    if (size == 0) {
        return 0;
    }

    size = double_word_align(size);
    r = next_free_region_of_size(mmr, size);
    if (r == 0) {
        return 0;
    }
    allocate_region(mmr, r, size);
    return r->data;
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
    decrease_remaining_space(mmr, size);
}

void append_region(MemoryManager *mmr, Region *end, unsigned int size) {
    end->free = 1;
    end->size = size - sizeof(Region);
    decrease_remaining_space(mmr, sizeof(Region));
}

Region *next_free_region_of_size(MemoryManager *mmr, unsigned int size) {
    Region *cursor = mmr->base_region;
    Region *sentinel = final_region(mmr);
    while (TRUE) {
        if (cursor->free && cursor->size >= size) {
            return cursor;
        }
        if (cursor == sentinel) {
            return 0;
        }
        cursor = next_region(cursor);
    }
}

Region *next_region(Region *current) {
        uintptr_t shift;
        shift = ((uintptr_t) current + sizeof(Region) + current->size);
        return ((Region *) shift);
}

Region *create_base_region(MemoryManager *mmr) {
    Region *r = (Region *) mmr->start_of_memory;
    r->free = 1;
    decrease_remaining_space(mmr, sizeof(Region));
    r->size = mmr->remaining_space;
    return r;
}

Region *final_region(MemoryManager *mmr) {
    Region *cursor = mmr->base_region;
    uintptr_t address = (uintptr_t) cursor->data + (uintptr_t) cursor->size;
    while(address < mmr->end_of_memory) {
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

void test_myFree(MemoryManager *test_mmr, void *ptr) {
    mmr = test_mmr;
    myFree(ptr);
}

void myFree(void *ptr) {
    Region *r;
    if (mmr == 0) {
        mmr = initialize_memory(start_address, TOTAL_SPACE);
    }

    if (ptr == 0) {
        return;
    }

    if (!is_valid_pointer(mmr, ptr)) {
        return;
    }
    r = region_for_pointer(ptr);
    r->free = 1;
    increase_remaining_space(mmr, r->size);
}

int is_valid_pointer(MemoryManager *mmr, void *ptr) {
    Region *cursor = mmr->base_region;
    while (cursor <= final_region(mmr)) {
        if (ptr == cursor->data) {
            return TRUE;
        }
        cursor = next_region(cursor);
    }
    return FALSE;
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
    if (r == mmr->base_region) {
        return FALSE;
    }
    return FALSE;
}

void memoryMap(void) {
    MemoryManager *mmr = (MemoryManager *) start_address;
    printf("%p: size: %d free: %d\n", mmr->base_region->data,
        mmr->base_region->size, mmr->base_region->free);
}
