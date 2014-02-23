
#ifndef MEMORY_H
#define MEMORY_H
#include <stdint.h>

struct Region {
    /* 4 bytes */
    /* 1 bit for free and 31 bits for size */
    uint32_t free:1;
    uint32_t size:31;

    /* 4 bytes */
    /* id of the process that allocated this region */
    uint32_t pid;

    /* total 8 bytes */
    /* we know that the actual region starts 8 bytes after bookkeeping. */
    /* implicit pointer that always is at the end of the struct */
    uint8_t data[0];
};
typedef struct Region Region;

/*
void *myMalloc(unsigned int size);

void myFree(void *ptr);
*/

uint32_t getCurrentPID(void);


#endif

