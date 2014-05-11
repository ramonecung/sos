
#ifndef MEMORY_H
#define MEMORY_H
#include <stdint.h>
#include "../freescaleK70/hardware/sdram.h"

#if defined __linux__ || defined __APPLE__|| defined _WIN32 || defined _WIN64
#else
typedef unsigned long uintptr_t;
#endif

/* if we are on the K70 use SDRAM_SIZE */
/* if on a development system use 1M */
#ifdef K70
#define TOTAL_SPACE SDRAM_SIZE
#define SYSTEM_MEMORY_ADDRESS SDRAM_START
#else
#define TOTAL_SPACE 1048576
#endif

#define MAX_ALLOCATABLE_SPACE (TOTAL_SPACE - (sizeof(MemoryManager) + sizeof(Region)))
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
    unsigned int remaining_space;
    Region *base_region;
    uintptr_t start_of_memory;
    uintptr_t end_of_memory;
};
typedef struct MemoryManager MemoryManager;


/* function declarations */
void initialize_memory(void);
MemoryManager *configure_memory(void *start_address,
                                unsigned int total_space);
void *myMalloc(unsigned int size);
void myFree(void *ptr);
void memoryMap(void);

void *test_myMalloc(MemoryManager *test_mmr, unsigned int size);
void test_myFree(MemoryManager *test_mmr, void *ptr);
void set_start_address(void *addr);
uint32_t getCurrentPID(void);

Region *create_base_region(MemoryManager *mmr);
unsigned int double_word_align(unsigned int size);
void allocate_region(MemoryManager *mmr, Region *r, unsigned int size);
void append_region(MemoryManager *mmr, Region *end, unsigned int size);

Region *next_free_region_of_size(MemoryManager *mmr, unsigned int size);
Region *next_region(Region *current);
Region *previous_region(MemoryManager *mmr, Region *current);

int is_valid_pointer(MemoryManager *mmr, void *ptr);
Region *region_for_pointer(void *ptr);
Region *final_region(MemoryManager *mmr);

unsigned int remaining_space(MemoryManager *mmr);
void decrease_remaining_space(MemoryManager *mmr, unsigned int size);
void increase_remaining_space(MemoryManager *mmr, unsigned int size);

int can_merge_next(MemoryManager *mmr, Region *r);
void merge_next(MemoryManager *mmr, Region *r);
int can_merge_previous(MemoryManager *mmr, Region *r);
void merge_previous(MemoryManager *mmr, Region *r);

#endif

