#include "gtest/gtest.h"

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include "../util/util.h"
}

class UtilTest : public ::testing::Test {
    protected:

  // You can remove any or all of the following functions if its body
  // is empty.


  UtilTest() {
    // You can do set-up work for each test here.
  }

  virtual ~UtilTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
};

TEST_F(UtilTest, StringsEqual) {
    int res;
    char str1[] = "dog";
    char str2[] = "dog";
    char str3[] = "cat";
    char str4[] = "doggone";

    res = strings_equal(str1, str2);
    EXPECT_EQ(1, res);
    res = strings_equal(str1, str3);
    EXPECT_EQ(0, res);
    res = strings_equal(str1, str4);
    EXPECT_EQ(0, res);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
