
#ifndef MEMORY_H
#define MEMORY_H
#include <stdint.h>

#define TOTAL_SPACE 1000000
#define MAX_ALLOCATABLE_SPACE (TOTAL_SPACE - sizeof(MemoryManager))
#define WORD_SIZE 4

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

struct MemoryManager {
    unsigned long remaining_space;
    Region *base_region;
    Region *leading_edge;
};
typedef struct MemoryManager MemoryManager;

void *myMalloc(unsigned int size);
Region *allocate_region(MemoryManager *mmr, unsigned int size);

MemoryManager *initialize_memory(void *start_address,
                                unsigned long total_space);
void reduce_available_space(MemoryManager *mmr, unsigned int size);

unsigned int adjust_size(unsigned int size);
Region *region_for_pointer(void *ptr);
unsigned int remaining_space(MemoryManager *mmr);
void reduce_available_space_by(unsigned int size);
void set_start_address(void *addr);
int cannot_allocate(MemoryManager *mmr, unsigned int size);
/*
void myFree(void *ptr);
*/

uint32_t getCurrentPID(void);


#endif

