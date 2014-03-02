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


/*
If the storage cannot be
successfully allocated for any reason, the function should return a
NULL pointer (i.e., the value 0).
*/
TEST_F(MemoryTest, CannotAllocate) {
    void *ptr, *ptr2;
    EXPECT_EQ(TRUE, cannot_allocate(test_mmr, MAX_ALLOCATABLE_SPACE + 1));
    EXPECT_EQ(NULL, myMalloc(MAX_ALLOCATABLE_SPACE + 1));

    /* shouldn't be able to allocate half of all space twice */
    /* since the region takes up room */
    ptr = myMalloc(MAX_ALLOCATABLE_SPACE / 2);
    ptr2 = myMalloc(MAX_ALLOCATABLE_SPACE / 2);
    EXPECT_EQ(NULL, ptr2);
}

TEST_F(MemoryTest, ReduceAvailableSpace) {
    reduce_available_space(test_mmr, MAX_ALLOCATABLE_SPACE);
    EXPECT_EQ(0, remaining_space(test_mmr));

    test_mmr = initialize_memory(addr, TOTAL_SPACE);
    reduce_available_space(test_mmr, MAX_ALLOCATABLE_SPACE / 2);
    EXPECT_EQ(MAX_ALLOCATABLE_SPACE / 2, remaining_space(test_mmr));

    reduce_available_space(test_mmr, MAX_ALLOCATABLE_SPACE / 2);
    EXPECT_EQ(0, remaining_space(test_mmr));
}

TEST_F(MemoryTest, ShiftLeadingEdge) {
    Region *le1 = test_mmr->leading_edge;
    shift_leading_edge(test_mmr, 50);
    Region *le2 = test_mmr->leading_edge;
    EXPECT_EQ(50, (uintptr_t) le2 - (uintptr_t) le1);
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
    Region *region;
    region = allocate_region(test_mmr, 1);
    EXPECT_EQ(0, region->free);
    EXPECT_EQ(1, region->size);

    region = allocate_region(test_mmr, MAX_ALLOCATABLE_SPACE);
    EXPECT_EQ(0, region->free);
    EXPECT_EQ(MAX_ALLOCATABLE_SPACE, region->size);
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
TEST_F(MemoryTest, AdjustSize) {
    EXPECT_EQ(0, adjust_size(0));
    EXPECT_EQ(8, adjust_size(1));
    EXPECT_EQ(8, adjust_size(7));
    EXPECT_EQ(8, adjust_size(8));
    EXPECT_EQ(16, adjust_size(9));
    EXPECT_EQ(16, adjust_size(15));
    EXPECT_EQ(16, adjust_size(16));
    EXPECT_EQ(24, adjust_size(17));
}


/*
free
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
TEST_F(MemoryTest, DISABLED_FreeInvalidPtr) {
    /* we know test_mmr is an initialized pointer that does not point
     * to an allocated region of memory */
    void *ptr = (void *) test_mmr;
    EXPECT_NO_THROW(myFree(ptr));
}

TEST_F(MemoryTest, IsAllocated) {
    void *ptr;
    ptr = test_mmr;
    int size = 8;
    EXPECT_EQ(FALSE, is_allocated(test_mmr, ptr));
    ptr = myMalloc(size);
    EXPECT_EQ(TRUE, is_allocated(test_mmr, ptr));

    ptr = myMalloc(size);
    EXPECT_EQ(TRUE, is_allocated(test_mmr, ptr));

    ptr = myMalloc(size);
    EXPECT_EQ(TRUE, is_allocated(test_mmr, ptr));
}

TEST_F(MemoryTest, IsAllocatedBound) {
    void *ptr;
    int size = MAX_ALLOCATABLE_SPACE;
    ptr = test_myMalloc(test_mmr, size);
    EXPECT_EQ(TRUE, is_allocated(test_mmr, ptr));
}

/*
The myFree function
should make the freed storage be available for a future call of
myMalloc.
*/
TEST_F(MemoryTest, FreeStorageAvailable) {
    int size = 8;
    void *ptr = myMalloc(size);
    Region *r = region_for_pointer(ptr);
    EXPECT_EQ(0, r->free);
    myFree(ptr);
    EXPECT_EQ(1, r->free);
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
