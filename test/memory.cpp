#include "gtest/gtest.h"

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include "../memory/memory.h"
#include "../include/constants.h"
}

class MemoryTest : public ::testing::Test {
    protected:

  // You can remove any or all of the following functions if its body
  // is empty.
        void *addr;
        MemoryManager *test_mmr;

  MemoryTest() {
    // You can do set-up work for each test here.
  }

  virtual ~MemoryTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
    addr = malloc(TOTAL_SPACE);
    if (addr == NULL) {
        printf("SetUp could not allocate memory\n");
        exit(1);
    }
    set_start_address(addr);
    test_mmr = initialize_memory(addr, TOTAL_SPACE);
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
    free(addr);
  }
};


TEST_F(MemoryTest, GetCurrentPID) {
    EXPECT_EQ(0, getCurrentPID());
}


TEST_F(MemoryTest, InitializeMemory) {
    test_mmr = initialize_memory(addr, TOTAL_SPACE);
    EXPECT_EQ(addr, test_mmr);
    EXPECT_EQ((uintptr_t) addr + TOTAL_SPACE, test_mmr->end_of_memory);
}

/*
If the storage cannot be
successfully allocated for any reason, the function should return a
NULL pointer (i.e., the value 0).
*/
TEST_F(MemoryTest, CannotAllocate) {
    void *ptr, *ptr2;
    EXPECT_EQ(NULL, myMalloc(MAX_ALLOCATABLE_SPACE + 1));

    /* shouldn't be able to allocate half of all space twice */
    /* since the region takes up room */
    ptr = myMalloc(MAX_ALLOCATABLE_SPACE / 2);
    ptr2 = myMalloc(MAX_ALLOCATABLE_SPACE / 2);
    EXPECT_EQ(NULL, ptr2);
}

TEST_F(MemoryTest, NextFreeRegionOfSize) {
    Region *r1, *r2;
    void *ptr;
    int size = 8;
    r1 = next_free_region_of_size(test_mmr, MAX_ALLOCATABLE_SPACE);
    EXPECT_EQ(test_mmr->base_region, r1);
    ptr = test_myMalloc(test_mmr, size);
    r1 = region_for_pointer(ptr);
    EXPECT_EQ(0, r1->free);
    r2 = next_free_region_of_size(test_mmr, size);
    EXPECT_EQ(1, r2->free);
}

TEST_F(MemoryTest, DecreaseRemainingSpace) {
    decrease_remaining_space(test_mmr, MAX_ALLOCATABLE_SPACE);
    EXPECT_EQ(0, remaining_space(test_mmr));

    test_mmr = initialize_memory(addr, TOTAL_SPACE);
    decrease_remaining_space(test_mmr, MAX_ALLOCATABLE_SPACE / 2);
    EXPECT_EQ(MAX_ALLOCATABLE_SPACE / 2, remaining_space(test_mmr));

    decrease_remaining_space(test_mmr, MAX_ALLOCATABLE_SPACE / 2);
    EXPECT_EQ(0, remaining_space(test_mmr));
}

TEST_F(MemoryTest, FinalRegion) {
    Region *r = test_mmr->base_region;
    int size = 128;
    EXPECT_EQ(r, final_region(test_mmr));
    allocate_region(test_mmr, r, size);
    EXPECT_EQ((Region *) (r->data + (uintptr_t) size), final_region(test_mmr));
    r = next_region(r);
    allocate_region(test_mmr, r, size);
    EXPECT_EQ((Region *) (r->data + (uintptr_t) size), final_region(test_mmr));
}



/*
If a request is made to allocate 0
bytes of memory, the function should return a NULL pointer.
*/
TEST_F(MemoryTest, RequestZeroGetNull) {
    EXPECT_EQ(NULL, myMalloc(0));
}

/*
should allocate an appropriately sized
region of memory
*/
TEST_F(MemoryTest, AllocateRegion) {
    allocate_region(test_mmr, test_mmr->base_region, 1);
    EXPECT_EQ(0, test_mmr->base_region->free);
    EXPECT_EQ(1, test_mmr->base_region->size);

    EXPECT_EQ(MAX_ALLOCATABLE_SPACE - 1 - sizeof(Region), remaining_space(test_mmr));

    /* reset */
    test_mmr->base_region->free = 1;
    test_mmr->base_region->size = MAX_ALLOCATABLE_SPACE;
    increase_remaining_space(test_mmr, 1 + sizeof(Region));

    allocate_region(test_mmr, test_mmr->base_region, MAX_ALLOCATABLE_SPACE);
    EXPECT_EQ(0, test_mmr->base_region->free);
    EXPECT_EQ(MAX_ALLOCATABLE_SPACE, test_mmr->base_region->size);
    EXPECT_EQ(0, remaining_space(test_mmr));
}

TEST_F(MemoryTest, AppendRegion) {
    Region *region = test_mmr->base_region;
    unsigned int original_size = remaining_space(test_mmr);
    unsigned int size = 512;
    Region *end_of_current = (Region *) (region->data + (uintptr_t) size);
    append_region(test_mmr, end_of_current, size);
    EXPECT_EQ(size - sizeof(Region), end_of_current->size);
    EXPECT_EQ(1, end_of_current->free);
    EXPECT_EQ(original_size - sizeof(Region), remaining_space(test_mmr));
}

/*
it should return a pointer to (i.e., the address
of) the first byte of that region.
*/
TEST_F(MemoryTest, ReturnedAddress) {
    int size = 8;
    Region *r = test_mmr->base_region;
    void *ptr = myMalloc(size);
    EXPECT_EQ((void *) r->data, ptr);
    void *ptr2 = myMalloc(size);
    uintptr_t shift = size + sizeof(Region);
    EXPECT_EQ((void *) (r->data + shift), ptr2);
}

TEST_F(MemoryTest, RegionForPointer) {
    void *ptr = myMalloc(8);
    Region *r = region_for_pointer(ptr);
    EXPECT_EQ(((Region *) ptr - 1), r);
    EXPECT_EQ(0, r->free);
    EXPECT_EQ(8, r->size);
}


/*
The pointer returned by myMalloc should always point to a region of
memory that starts on an 8-byte (double-word) boundary.
*/
TEST_F(MemoryTest, DoubleWordAlign) {
    EXPECT_EQ(0, double_word_align(0));
    EXPECT_EQ(8, double_word_align(1));
    EXPECT_EQ(8, double_word_align(7));
    EXPECT_EQ(8, double_word_align(8));
    EXPECT_EQ(16, double_word_align(9));
    EXPECT_EQ(16, double_word_align(15));
    EXPECT_EQ(16, double_word_align(16));
    EXPECT_EQ(24, double_word_align(17));
}


/*
myFree
The "ptr" parameter is a pointer to a region of memory previously
allocated by the myMalloc function.
*/

/*
If the value
of the parameter is NULL, the call has no effect.
*/
TEST_F(MemoryTest, FreeNull) {
    EXPECT_NO_THROW(myFree(NULL));
}
/*
If the "ptr"
parameter does not point to a previously allocated region of memory,
the call should have no effect.
*/
TEST_F(MemoryTest, FreeInvalidPtr) {
    /* we know test_mmr is an initialized pointer that does not point
     * to an allocated region of memory */
    void *ptr = (void *) test_mmr;
    EXPECT_NO_THROW(myFree(ptr));
}

TEST_F(MemoryTest, IsValidPointer) {
    void *ptr;
    ptr = test_mmr;
    int size = 8;
    EXPECT_EQ(FALSE, is_valid_pointer(test_mmr, ptr));

    ptr = myMalloc(size);
    EXPECT_EQ(TRUE, is_valid_pointer(test_mmr, ptr));

    ptr = myMalloc(size);
    EXPECT_EQ(TRUE, is_valid_pointer(test_mmr, ptr));

    ptr = myMalloc(size);
    EXPECT_EQ(TRUE, is_valid_pointer(test_mmr, ptr));
}

TEST_F(MemoryTest, IsAllocatedBound) {
    void *ptr;
    int size = MAX_ALLOCATABLE_SPACE;
    ptr = test_myMalloc(test_mmr, size);
    EXPECT_EQ(TRUE, is_valid_pointer(test_mmr, ptr));
}

/*
The myFree function
should make the freed storage be available for a future call of
myMalloc.
*/
TEST_F(MemoryTest, FreeStorageAvailable) {
    int size = 8;
    void *ptr1, *ptr2;
    ptr1 = test_myMalloc(test_mmr, size);
    Region *r = region_for_pointer(ptr1);
    EXPECT_EQ(0, r->free);
    myFree(ptr1);
    EXPECT_EQ(1, r->free);
    ptr2 = test_myMalloc(test_mmr, size);
    EXPECT_EQ(ptr2, ptr1);
}

TEST_F(MemoryTest, RemainingSpaceIncreases) {
    void *ptr1, *ptr2;
    int size = 8;
    unsigned int original_remaining_space;
    unsigned int intermediate_remaining_space;

    original_remaining_space = remaining_space(test_mmr);
    ptr1 = test_myMalloc(test_mmr, size);

    intermediate_remaining_space = remaining_space(test_mmr);
    EXPECT_LT(intermediate_remaining_space, original_remaining_space);

    ptr2 = test_myMalloc(test_mmr, size);
    EXPECT_LT(remaining_space(test_mmr), intermediate_remaining_space);

    myFree(ptr2);
    EXPECT_EQ(intermediate_remaining_space - sizeof(Region), remaining_space(test_mmr));

    myFree(ptr1);
    EXPECT_EQ(original_remaining_space - (2 * sizeof(Region)), remaining_space(test_mmr));
}

TEST_F(MemoryTest, ContiguousAvailable) {
    void *p1, *p2, *p3, *p4;
    /* want to divide available memory into 4 pieces */
    /* already have the base region, will need 3 more */
    unsigned int total = TOTAL_SPACE - sizeof(MemoryManager);
    unsigned int parcel_with_region = total / 4;
    unsigned int parcel = parcel_with_region - sizeof(Region);
    parcel = parcel & ~7; /* round down since we will double word align */
    p1 = test_myMalloc(test_mmr, parcel);
    p2 = test_myMalloc(test_mmr, parcel);
    p3 = test_myMalloc(test_mmr, parcel);
    p4 = test_myMalloc(test_mmr, parcel);

    /* free two non-contiguous parcels */
    test_myFree(test_mmr, p2);
    test_myFree(test_mmr, p4);
    EXPECT_LE(2 * parcel, remaining_space(test_mmr));

    /* we have 2 * parcel free space but we cannot allocate it in one block */
    EXPECT_EQ(NULL, test_myMalloc(test_mmr, (2 * parcel)));
}


/*
Only the process that allocated a particular
region of memory will be allowed to successfully deallocate that
region using myFree.
*/


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
