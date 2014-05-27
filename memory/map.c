#ifdef MEMORY_DEMO

#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include "../freescaleK70/io.h"
#include "../include/svc.h"
#include "../util/util.h"

#define OUTPUT_LENGTH 64

int main(void) {
    char formatted_output[OUTPUT_LENGTH];
    MemoryManager *mmr;
    void *first_ptr, *last_ptr, *mid_ptr, *surrounded_ptr, *ptr;
    /* get the test region from the system */
    void *swath = malloc(TOTAL_SPACE);
    if (swath == NULL) {
        return 1;
    }

    mmr = initialize_memory(swath, TOTAL_SPACE);
    efputs("Memory region info:\n", STDOUT);

    sprintf(formatted_output, "First allocatable address: %p\n",
        mmr->base_region->data);
    efputs(formatted_output, STDOUT);

    sprintf(formatted_output, "Ending border: %p\n",
        (void *) mmr->end_of_memory);
    efputs(formatted_output, STDOUT);

    sprintf(formatted_output, "Initial available space: %d\n",
        remaining_space(mmr));
    efputs(formatted_output, STDOUT);

    efputs("\n", STDOUT);
    efputs("Initial memory map:\n", STDOUT);
    memoryMap();
    efputs("\n", STDOUT);
    efputs("Series of memory allocations...\n", STDOUT);
    efputs("\n", STDOUT);

    efputs("first_ptr = myMalloc(32);\n", STDOUT);
    first_ptr = myMalloc(32);
    memoryMap();
    efputs("\n", STDOUT);

    efputs("surrounded_ptr = myMalloc(1024);\n", STDOUT);
    surrounded_ptr = myMalloc(1024);
    memoryMap();
    efputs("\n", STDOUT);

    efputs("mid_ptr = myMalloc(8);\n", STDOUT);
    mid_ptr = myMalloc(8);
    memoryMap();
    efputs("\n", STDOUT);

    efputs("ptr = myMalloc(1);\n", STDOUT);
    ptr = myMalloc(1);
    memoryMap();
    efputs("\n", STDOUT);

    efputs("ptr = myMalloc(0);\n", STDOUT);
    ptr = myMalloc(0);
    memoryMap();
    efputs("\n", STDOUT);


    efputs("last_ptr = myMalloc(1047432);\n", STDOUT);
    last_ptr = myMalloc(1047432);
    memoryMap();
    efputs("\n", STDOUT);

    efputs("Series of memory frees...\n", STDOUT);
    efputs("\n", STDOUT);

    efputs("myFree(last_ptr);\n", STDOUT);
    myFree(last_ptr);
    memoryMap();
    efputs("\n", STDOUT);

    efputs("myFree(first_ptr);\n", STDOUT);
    myFree(first_ptr);
    memoryMap();
    efputs("\n", STDOUT);

    efputs("myFree(mid_ptr);\n", STDOUT);
    myFree(mid_ptr);
    memoryMap();
    efputs("\n", STDOUT);

    efputs("myFree(surrounded_ptr);\n", STDOUT);
    myFree(surrounded_ptr);
    memoryMap();
    efputs("\n", STDOUT);

    return 0;
}

#endif
