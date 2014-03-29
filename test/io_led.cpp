#include "gtest/gtest.h"

extern "C" {
#include <stdio.h>
#include "../freescaleK70/io_led.h"
}

class IOLedTest : public ::testing::Test {
    protected:

  // You can remove any or all of the following functions if its body
  // is empty.

    Stream *test_stream;

  IOLedTest() {
    // You can do set-up work for each test here.
  }

  virtual ~IOLedTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
    test_stream = (Stream *) malloc(sizeof(Stream));
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
};

TEST_F(IOLedTest, Fgetc) {
    EXPECT_EQ(0, fgetc_led(test_stream));
}

TEST_F(IOLedTest, Fputc) {
    int c = 'c';
    EXPECT_EQ(c, fputc_led(c, test_stream));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
