#include <stdlib.h>
#include <stdio.h>
#include "memory.h"

int main(void) {
    MemoryManager *mmr;
    void *first_ptr, *last_ptr, *mid_ptr, *surrounded_ptr, *ptr;
    /* get the test region from the system */
    void *swath = malloc(TOTAL_SPACE);
    if (swath == NULL) {
        return 1;
    }

    mmr = initialize_memory(swath, TOTAL_SPACE);
    printf("Memory region info:\n");
    printf("First allocatable address: %p\n", mmr->base_region->data);
    printf("Ending border: %p\n", (void *) mmr->end_of_memory);
    printf("Initial available space: %d\n", remaining_space(mmr));
    printf("\n");
    printf("Initial memory map:\n");
    memoryMap();
    printf("\n");
    printf("Series of memory allocations...\n");
    printf("\n");

    printf("first_ptr = myMalloc(32);\n");
    first_ptr = myMalloc(32);
    memoryMap();
    printf("\n");

    printf("surrounded_ptr = myMalloc(1024);\n");
    surrounded_ptr = myMalloc(1024);
    memoryMap();
    printf("\n");

    printf("mid_ptr = myMalloc(8);\n");
    mid_ptr = myMalloc(8);
    memoryMap();
    printf("\n");

    printf("ptr = myMalloc(1);\n");
    ptr = myMalloc(1);
    memoryMap();
    printf("\n");

    printf("ptr = myMalloc(0);\n");
    ptr = myMalloc(0);
    memoryMap();
    printf("\n");


    printf("last_ptr = myMalloc(1047432);\n");
    last_ptr = myMalloc(1047432);
    memoryMap();
    printf("\n");

    printf("Series of memory frees...\n");
    printf("\n");

    printf("myFree(last_ptr);\n");
    myFree(last_ptr);
    memoryMap();
    printf("\n");

    printf("myFree(first_ptr);\n");
    myFree(first_ptr);
    memoryMap();
    printf("\n");

    printf("myFree(mid_ptr);\n");
    myFree(mid_ptr);
    memoryMap();
    printf("\n");

    printf("myFree(surrounded_ptr);\n");
    myFree(surrounded_ptr);
    memoryMap();
    printf("\n");

    return 0;
}
