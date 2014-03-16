#include "gtest/gtest.h"

extern "C" {
#include "../freescaleK70/button_flash.h"
}

class ButtonFlashTest : public ::testing::Test {
    protected:

  // You can remove any or all of the following functions if its body
  // is empty.


  ButtonFlashTest() {
    // You can do set-up work for each test here.
  }

  virtual ~ButtonFlashTest() {
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

TEST_F(ButtonFlashTest, Initialize) {
    bfInitialize();
    EXPECT_EQ(ON, orange_status());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

