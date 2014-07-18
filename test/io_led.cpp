extern "C" {
#include "../include/io.h"
#include "../io/io_led.h"
#include "../memory/memory.h"
#include "../util/util.h"
}

#include "gtest/gtest.h"
#include "../third-party/fff.h"
DEFINE_FFF_GLOBALS;

FAKE_VOID_FUNC(ledOrangeOn);
FAKE_VOID_FUNC(ledYellowOn);
FAKE_VOID_FUNC(ledGreenOn);
FAKE_VOID_FUNC(ledBlueOn);
FAKE_VOID_FUNC(ledOrangeOff);
FAKE_VOID_FUNC(ledYellowOff);
FAKE_VOID_FUNC(ledGreenOff);
FAKE_VOID_FUNC(ledBlueOff);
FAKE_VOID_FUNC(ledInitAll);

FAKE_VOID_FUNC(disable_interrupts);
FAKE_VOID_FUNC(enable_interrupts);
FAKE_VALUE_FUNC(uint32_t, getCurrentPID);

class IOLedTest : public ::testing::Test {
  protected:

  // You can remove any or all of the following functions if its body
  // is empty.

  Stream *test_stream;
  Stream ts;

  IOLedTest() {
    // You can do set-up work for each test here.
    initialize_memory();
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
    RESET_FAKE(ledYellowOn);
    RESET_FAKE(ledGreenOn);
    RESET_FAKE(ledBlueOn);

    RESET_FAKE(ledOrangeOff);
    RESET_FAKE(ledYellowOff);
    RESET_FAKE(ledGreenOff);
    RESET_FAKE(ledBlueOff);

    RESET_FAKE(ledInitAll);

    RESET_FAKE(disable_interrupts);
    RESET_FAKE(enable_interrupts);
    RESET_FAKE(getCurrentPID);

    FFF_RESET_HISTORY();

    ts.device_instance = LED_ORANGE;
    test_stream = &ts;
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
};

TEST_F(IOLedTest, InitializeIOLed) {
  initialize_io_led();
  EXPECT_EQ(1, ledInitAll_fake.call_count);
}

TEST_F(IOLedTest, Fgetc) {
    EXPECT_EQ(EOF, fgetc_led());
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
