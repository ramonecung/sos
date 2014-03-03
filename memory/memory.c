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
    unsigned int additional_space_used;
    if (mmr == 0) {
        mmr = initialize_memory(start_address, TOTAL_SPACE);
    }

    if (size == 0) {
        return 0;
    }

    size = adjust_size(size);
    /* unless we are at the base region we need to account for a new region */
    if (mmr->base_region->free) {
        additional_space_used = size;
    } else {
        additional_space_used = size + sizeof(Region);
    }

    if (cannot_allocate(mmr, additional_space_used)) {
        return 0;
    }
    r = allocate_region(mmr, size);
    reduce_available_space(mmr, additional_space_used);
    shift_leading_edge(mmr, size + sizeof(Region));
    return r->data;
}

Region *allocate_region(MemoryManager *mmr, unsigned int size) {
    Region *r = mmr->leading_edge;
    r->size = size;
    r->free = 0;
    return r;
}

void reduce_available_space(MemoryManager *mmr, unsigned int size) {
    mmr->remaining_space -= size;
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
    mmr->remaining_space = total_space - (sizeof(MemoryManager) + sizeof(Region));
    mmr->base_region = (Region *) ((uintptr_t) start_address + sizeof(MemoryManager));
    mmr->base_region->free = 1;
    mmr->base_region->size = 0;
    mmr->leading_edge = mmr->base_region;
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

int cannot_allocate(MemoryManager *mmr, unsigned int size) {
    if (size > mmr->remaining_space) {
        return TRUE;
    } else {
        return FALSE;
    }
}



void myFree(void *ptr) {
    if (mmr == 0) {
        mmr = initialize_memory(start_address, TOTAL_SPACE);
    }

    if (ptr == 0) {
        return;
    }

    if (!is_valid_pointer(mmr, ptr)) {
        return;
    }
    Region *r = region_for_pointer(ptr);
    r->free = 1;
}

int is_valid_pointer(MemoryManager *mmr, void *ptr) {
    Region *cursor = mmr->base_region;
    while (cursor <= mmr->leading_edge) {
        if (ptr == cursor->data) {
            return TRUE;
        }
        cursor = next_region(cursor);
    }
    return FALSE;
}

Region *next_region(Region *current) {
        uintptr_t shift;
        shift = ((uintptr_t) current + sizeof(Region) + current->size);
        return ((Region *) shift);
}
