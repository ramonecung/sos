#include <stdint.h>
#include "memory.h"
#include "../include/constants.h"

static Region base_region;
static Region *current_region;

uint32_t getCurrentPID(void) {
    return 0;
}

void *myMalloc(unsigned int size) {
    void *rv;

    if (size == 0) {
        return 0;
    }
    if (cannot_allocate(size)) {
        return 0;
    }
    rv = (void *) current_region->data;
    allocate_region(current_region, size);
    move_current_region_forward(size);
    return rv;
}

void allocate_region(Region *region, unsigned int size) {
    region->size = size;
    region->free = 0;
}

void move_current_region_forward(unsigned int size) {
    unsigned int remaining_space = current_region->size - size;
    current_region = (Region *) (current_region->data + size);
    current_region->size = remaining_space;
}

Region *region_for_pointer(void *ptr) {
    return ((Region *) ptr - 1);
}

Region *get_base_region(void) {
    return &base_region;
}

void initialize_memory(void) {
    base_region.free = 1;
    base_region.size = TOTAL_SPACE - sizeof(Region);
    current_region = &base_region;
}

unsigned int adjust_size(unsigned int size) {
    unsigned int double_word_size = 2 * WORD_SIZE;
    unsigned int padding = double_word_size - 1;
    return (size + padding) & ~padding;
}

int cannot_allocate(unsigned int size) {
    size = adjust_size(size);
    if (size > TOTAL_SPACE) {
        return TRUE;
    } else {
        return FALSE;
    }
}
