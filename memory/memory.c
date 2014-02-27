#include <stdint.h>
#include "memory.h"

uint32_t getCurrentPID(void) {
    return 0;
}

void *myMalloc(unsigned int size) {
    return 0;
}

void allocate_region(Region *region, unsigned int size) {
    region->size = size;
    region->free = 0;
}
