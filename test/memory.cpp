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
    set_start_address(addr);
    test_mmr = initialize_memory(addr, TOTAL_SPACE);
    if (addr == NULL) {
        printf("SetUp could not allocate memory\n");
        exit(1);
    }
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
    EXPECT_EQ(TRUE, cannot_allocate(MAX_ALLOCATABLE_SPACE + 1));
    EXPECT_EQ(NULL, myMalloc(MAX_ALLOCATABLE_SPACE + 1));
}

TEST_F(MemoryTest, AvailableSpaceUpdates1) {
    Region r;
    allocate_region(test_mmr, &r, MAX_ALLOCATABLE_SPACE);
    EXPECT_EQ(0, remaining_space(test_mmr));
}

TEST_F(MemoryTest, AvailableSpaceUpdates2) {
    Region r;
    allocate_region(test_mmr, &r, MAX_ALLOCATABLE_SPACE / 2);
    EXPECT_EQ(MAX_ALLOCATABLE_SPACE / 2, remaining_space(test_mmr));
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
    Region region;
    allocate_region(test_mmr, &region, 1);
    EXPECT_EQ(0, region.free);
    EXPECT_EQ(1, region.size);

    allocate_region(test_mmr, &region, MAX_ALLOCATABLE_SPACE);
    EXPECT_EQ(0, region.free);
    EXPECT_EQ(MAX_ALLOCATABLE_SPACE, region.size);
}



TEST_F(MemoryTest, RegionForPointer) {
    void *ptr = myMalloc(8);
    Region *r = region_for_pointer(ptr);
    EXPECT_EQ(((Region *) ptr - 1), r);
    EXPECT_EQ(0, r->free);
    EXPECT_EQ(8, r->size);
}

/*
it should return a pointer to (i.e., the address
of) the first byte of that region.
*/
TEST_F(MemoryTest, ReturnedAddress) {
    Region *r = &(test_mmr->base_region);
    void *ptr = myMalloc(8);
    EXPECT_EQ((void *) r->data, ptr);
    //void *ptr2 = myMalloc(8);
    //EXPECT_EQ((void *) (sizeof(MemoryManager) + sizeof(Region) + 8), ptr2);
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

/*
If the "ptr"
parameter does not point to a previously allocated region of memory,
the call should have no effect.
*/

/*
The myFree function
should make the freed storage be available for a future call of
myMalloc.
*/


/*
Only the process that allocated a particular
region of memory will be allowed to successfully deallocate that
region using myFree.
*/


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
