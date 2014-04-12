extern "C" {
#include "../freescaleK70/io_button.h"
}

#include "gtest/gtest.h"
#include "../third-party/fff.h"
DEFINE_FFF_GLOBALS;
FAKE_VOID_FUNC(pushbuttonInitAll);
FAKE_VOID_FUNC(turn_on_button, enum device_instance);
FAKE_VOID_FUNC(turn_off_button, enum device_instance);

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
    RESET_FAKE(pushbuttonInitAll);
    RESET_FAKE(turn_on_button);
    RESET_FAKE(turn_off_button);
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

TEST_F(IOButtonTest, InitializeIOButton) {
    initialize_io_button();
    EXPECT_EQ(1, pushbuttonInitAll_fake.call_count);
}

TEST_F(IOButtonTest, Fopen) {
    Stream *s;
    s = fopen_button(BUTTON_SW1);
    EXPECT_EQ(BUTTON_SW1, s->device_instance);
}

TEST_F(IOButtonTest, Fclose) {
    int res;
    Stream *s;
    s = fopen_button(BUTTON_SW1);
    res = fclose_button(s);
    EXPECT_EQ(0, res);
}

TEST_F(IOButtonTest, Fgetc) {
    int c;
    c = fgetc_button();
    EXPECT_EQ(0, c);
}

TEST_F(IOButtonTest, Fputc) {
    int c = 'c';
    EXPECT_EQ('c', fputc_button(c, test_stream));
    EXPECT_EQ(1, turn_on_button_fake.call_count);
    EXPECT_EQ(0, turn_off_button_fake.call_count);
}

TEST_F(IOButtonTest, FputcZero) {
    int c = 0;
    EXPECT_EQ(0, fputc_button(c, test_stream));
    EXPECT_EQ(0, turn_on_button_fake.call_count);
    EXPECT_EQ(1, turn_off_button_fake.call_count);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
