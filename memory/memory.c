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

    size = adjust_size(size);
    if (!large_enough_region_available(mmr, size)) {
        return 0;
    }
    r = next_large_enough_region(mmr, size);
    allocate_region(r, size);
    decrease_remaining_space(mmr, size);
    shift_leading_edge(mmr, size);
    return r->data;
}

int large_enough_region_available(MemoryManager *mmr, unsigned int size) {
    Region *r, *s;
    r = next_free_region(mmr);
    while (r->size < size) {
        s = r;
        r = next_free_region(mmr);
        if (r == s || r == 0) {
            return 0;
        }
    }
    return 1;
}

Region *next_large_enough_region(MemoryManager *mmr, unsigned int size) {
    Region *r;
    do {
        r = next_free_region(mmr);
    } while (r->size < size);
    return r;
}

Region *next_free_region(MemoryManager *mmr) {
    Region *cursor = mmr->base_region;
    while (cursor < mmr->leading_edge) {
        if (cursor->free) {
            return cursor;
        }
        cursor = next_region(cursor);
    }
    return create_new_region(mmr);
}

Region *next_region(Region *current) {
        uintptr_t shift;
        shift = ((uintptr_t) current + sizeof(Region) + current->size);
        return ((Region *) shift);
}

Region *create_new_region(MemoryManager *mmr) {
    Region *r;
    if (space_at_end(mmr) >= sizeof(Region)) {
        r = mmr->leading_edge;
        r->free = 1;
        r->size = space_at_end(mmr) - sizeof(Region);
        decrease_remaining_space(mmr, sizeof(Region));
        shift_leading_edge(mmr, sizeof(Region));
        return r;
    } else {
        return 0;
    }
}

uintptr_t space_at_end(MemoryManager *mmr) {
    return (mmr->end_of_memory - (uintptr_t) mmr->leading_edge);
}

void allocate_region(Region *r, unsigned int size) {
    r->size = size;
    r->free = 0;
}

void decrease_remaining_space(MemoryManager *mmr, unsigned int size) {
    mmr->remaining_space -= size;
}

void increase_remaining_space(MemoryManager *mmr, unsigned int size) {
    mmr->remaining_space += size;
}

void shift_leading_edge(MemoryManager *mmr, unsigned int size) {
    uintptr_t shift = (uintptr_t) mmr->leading_edge;
    shift = shift + size;
    mmr->leading_edge = (Region *) shift;
}

Region *region_for_pointer(void *ptr) {
    return ((Region *) ptr - 1);
}

MemoryManager *initialize_memory(void *start_address,
                                unsigned int total_space) {
    MemoryManager *mmr = (MemoryManager *) start_address;
    mmr->end_of_memory = ((uintptr_t) start_address + total_space);
    mmr->remaining_space = total_space - (sizeof(MemoryManager));
    mmr->leading_edge = (Region *) ((uintptr_t) start_address + sizeof(MemoryManager));
    mmr->base_region = create_new_region(mmr);
    return mmr;
}

unsigned int adjust_size(unsigned int size) {
    unsigned int double_word_size = 2 * WORD_SIZE;
    unsigned int padding = double_word_size - 1;
    return (size + padding) & ~padding;
}

unsigned int remaining_space(MemoryManager *mmr) {
    return mmr->remaining_space;
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
    while (cursor < mmr->leading_edge) {
        if (ptr == cursor->data) {
            return TRUE;
        }
        cursor = next_region(cursor);
    }
    return FALSE;
}
