#include <stdint.h>
#include <stdio.h>
#include "memory.h"
#include "../util/util.h"
#include "../include/constants.h"


void *start_address;

uint32_t getCurrentPID(void) {
    return 0;
}

void set_start_address(void *addr) {
    start_address = addr;
}

void *myMalloc(unsigned int size) {
    static MemoryManager *mmr;
    void *rv;

    if (mmr == 0) {
        mmr = initialize_memory(start_address, TOTAL_SPACE);
    }

    if (size == 0) {
        return 0;
    }
    if (cannot_allocate(size)) {
        return 0;
    }
    rv = (void *) mmr->base_region.data;
    Region *newRegion = (Region *) mmr->leading_edge;
    allocate_region(mmr, newRegion, size);
    return rv;
}

void allocate_region(MemoryManager *mmr, Region *region, unsigned int size) {
    region->size = size;
    region->free = 0;
    mmr->remaining_space -= size;
    mmr->leading_edge = mmr->leading_edge + sizeof(Region) + size;
}

Region *region_for_pointer(void *ptr) {
    return ((Region *) ptr - 1);
}


MemoryManager *initialize_memory(void *start_address,
                                unsigned long total_space) {
    MemoryManager *mmr = (MemoryManager *) start_address;
    mmr->base_region.free = 1;
    mmr->base_region.size = 0;
    mmr->remaining_space = total_space - sizeof(MemoryManager);
    mmr->leading_edge = &(mmr->base_region);
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

int cannot_allocate(unsigned int size) {
    size = adjust_size(size);
    if (size > MAX_ALLOCATABLE_SPACE) {
        return TRUE;
    } else {
        return FALSE;
    }
}
