#include <stdlib.h>
#include <stdio.h>
#include "memory.h"

int main(void) {
    MemoryManager *mmr;
    /* get the test region from the system */
    void *swath = malloc(TOTAL_SPACE);
    if (swath == NULL) {
        return 1;
    }

    mmr = initialize_memory(swath, TOTAL_SPACE);
    printf("Initial region:\n");
    printf("Start address: %p\n", mmr->base_region->data);
    printf("End address: %p\n", (void *) mmr->end_of_memory);
    printf("\n");
    memoryMap();


    return 0;
}
