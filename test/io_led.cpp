extern "C" {
#include "../freescaleK70/io_led.h"
}

#include "gtest/gtest.h"
#include "../third-party/fff.h"
DEFINE_FFF_GLOBALS;

FAKE_VOID_FUNC(ledOrangeOn);
FAKE_VOID_FUNC(ledOrangeOff);

class IOLedTest : public ::testing::Test {
  protected:

  // You can remove any or all of the following functions if its body
  // is empty.

  Stream *test_stream;
  Stream ts;
  Device d;

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
    RESET_FAKE(ledOrangeOn);
    RESET_FAKE(ledOrangeOff);
    FFF_RESET_HISTORY();

    ts.device = &d;
    ts.device_instance = LED_ORANGE;
    test_stream = &ts;
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
};

TEST_F(IOLedTest, Fgetc) {
    EXPECT_EQ(0, fgetc_led());
}

TEST_F(IOLedTest, Fputc) {
    int c, res;
    c = 'c';
    res = fputc_led(c, test_stream);
    EXPECT_EQ(c, res);
    EXPECT_EQ(1, ledOrangeOn_fake.call_count);
    EXPECT_EQ(0, ledOrangeOff_fake.call_count);

    c = 0;
    res = fputc_led(c, test_stream);
    EXPECT_EQ(c, res);
    EXPECT_EQ(1, ledOrangeOff_fake.call_count);
    /* should not have called "on" again */
    EXPECT_EQ(1, ledOrangeOn_fake.call_count);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
