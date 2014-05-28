extern "C" {
#include "../include/io.h"
#include "../io/io_button.h"
#include "../memory/memory.h"
#include "../util/util.h"
}

#include "gtest/gtest.h"
#include "../third-party/fff.h"
DEFINE_FFF_GLOBALS;
FAKE_VOID_FUNC(pushbuttonInitAll);
FAKE_VALUE_FUNC(int, sw1In);
FAKE_VALUE_FUNC(int, sw2In);

class IOButtonTest : public ::testing::Test {
  protected:

  // You can remove any or all of the following functions if its body
  // is empty.

  Stream *test_stream;
  Stream ts;

  IOButtonTest() {
    // You can do set-up work for each test here.
    initialize_memory();
  }

  virtual ~IOButtonTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
    RESET_FAKE(pushbuttonInitAll);
    RESET_FAKE(sw1In);
    RESET_FAKE(sw2In);
    FFF_RESET_HISTORY();

    test_stream = &ts;
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
};

TEST_F(IOButtonTest, InitializeIOButton) {
    initialize_io_button();
    EXPECT_EQ(1, pushbuttonInitAll_fake.call_count);
}

TEST_F(IOButtonTest, Fgetc) {
    int c;
    test_stream->device_instance = BUTTON_SW1;
    c = fgetc_button(test_stream);
    EXPECT_EQ(1, sw1In_fake.call_count);
    test_stream->device_instance = BUTTON_SW2;
    c = fgetc_button(test_stream);
    EXPECT_EQ(1, sw2In_fake.call_count);
}

TEST_F(IOButtonTest, Fputc) {
    int c = 'c';
    EXPECT_EQ('c', fputc_button(c));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
