extern "C" {
#include "../freescaleK70/io.h"
}

#include "gtest/gtest.h"
#include "../third-party/fff.h"
DEFINE_FFF_GLOBALS;
FAKE_VOID_FUNC(pushbuttonInitAll);
FAKE_VALUE_FUNC(int, sw1In);
FAKE_VALUE_FUNC(int, sw2In);
FAKE_VOID_FUNC(ledOrangeOn);
FAKE_VOID_FUNC(ledOrangeOff);
FAKE_VOID_FUNC(ledInitAll);

class IOTest : public ::testing::Test {
  protected:

  // You can remove any or all of the following functions if its body
  // is empty.

  Stream *test_stream;
  Stream ts;
  Device d;

  IOTest() {
    // You can do set-up work for each test here.
  }

  virtual ~IOTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
    RESET_FAKE(ledOrangeOn);
    RESET_FAKE(ledOrangeOff);
    RESET_FAKE(pushbuttonInitAll);
    RESET_FAKE(sw1In);
    RESET_FAKE(sw2In);
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

TEST_F(IOTest, myFopen) {
    test_stream = myFopen("BUTTON_SW1");
    EXPECT_EQ(BUTTON_SW1, test_stream->device_instance);
    myFclose(test_stream);

    test_stream = myFopen("LED_ORANGE");
    EXPECT_EQ(LED_ORANGE, test_stream->device_instance);
    myFclose(test_stream);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}