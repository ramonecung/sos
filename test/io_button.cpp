extern "C" {
#include "../freescaleK70/io_button.h"
}

#include "gtest/gtest.h"
#include "../third-party/fff.h"
DEFINE_FFF_GLOBALS;

class IOButtonTest : public ::testing::Test {
  protected:

  // You can remove any or all of the following functions if its body
  // is empty.

  Stream *test_stream;
  Stream ts;
  Device d;

  IOButtonTest() {
    // You can do set-up work for each test here.
  }

  virtual ~IOButtonTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
    FFF_RESET_HISTORY();

    ts.device = &d;
    ts.device_instance = BUTTON_SW1;
    test_stream = &ts;
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
};

TEST_F(IOButtonTest, Fopen) {
    Stream *s;
    s = fopen_button(BUTTON_SW1);
    EXPECT_EQ(BUTTON_SW1, s->device_instance);
}

/*
TEST_F(IOButtonTest, Fclose) {
}

TEST_F(IOButtonTest, Fgetc) {
}

TEST_F(IOButtonTest, Fputc) {
}
*/
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
