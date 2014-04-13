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
FAKE_VALUE_FUNC(Stream *, fopen_button, enum device_instance);
FAKE_VALUE_FUNC(int, fclose_button, Stream *);
FAKE_VALUE_FUNC(int, fgetc_button, Stream *);
FAKE_VALUE_FUNC(Stream *, fopen_led, enum device_instance);
FAKE_VALUE_FUNC(int, fclose_led, Stream *);
FAKE_VALUE_FUNC(int, fgetc_led);

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

    RESET_FAKE(fopen_button);
    RESET_FAKE(fclose_button);
    RESET_FAKE(fgetc_button);

    RESET_FAKE(fopen_led);
    RESET_FAKE(fclose_led);
    RESET_FAKE(fgetc_led);
    FFF_RESET_HISTORY();

    ts.device = &d;
    test_stream = &ts;
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
};

TEST_F(IOTest, MyFopen) {
    test_stream = myFopen("BUTTON_SW1");
    EXPECT_EQ(BUTTON_SW1, fopen_button_fake.arg0_history[0]);
    EXPECT_EQ(1, fopen_button_fake.call_count);

    test_stream = myFopen("BUTTON_SW2");
    EXPECT_EQ(BUTTON_SW2, fopen_button_fake.arg0_history[1]);

    test_stream = myFopen("LED_ORANGE");
    EXPECT_EQ(LED_ORANGE, fopen_led_fake.arg0_history[0]);

    test_stream = myFopen("LED_YELLOW");
    EXPECT_EQ(LED_YELLOW, fopen_led_fake.arg0_history[1]);

    test_stream = myFopen("LED_GREEN");
    EXPECT_EQ(LED_GREEN, fopen_led_fake.arg0_history[2]);

    test_stream = myFopen("LED_BLUE");
    EXPECT_EQ(LED_BLUE, fopen_led_fake.arg0_history[3]);
}

TEST_F(IOTest, MyFclose) {
    ts.device_instance = BUTTON_SW1;
    EXPECT_EQ(0, myFclose(test_stream));

    ts.device_instance = BUTTON_SW2;
    EXPECT_EQ(0, myFclose(test_stream));

    ts.device_instance = LED_ORANGE;
    EXPECT_EQ(0, myFclose(test_stream));

    ts.device_instance = LED_YELLOW;
    EXPECT_EQ(0, myFclose(test_stream));

    ts.device_instance = LED_GREEN;
    EXPECT_EQ(0, myFclose(test_stream));

    ts.device_instance = LED_BLUE;
    EXPECT_EQ(0, myFclose(test_stream));
}

TEST_F(IOTest, MyFgetc) {
    int c;
    ts.device_instance = BUTTON_SW1;
    c = myFgetc(test_stream);
    EXPECT_EQ(1, fgetc_button_fake.call_count);

    ts.device_instance = BUTTON_SW2;
    c = myFgetc(test_stream);
    EXPECT_EQ(2, fgetc_button_fake.call_count);

    ts.device_instance = LED_ORANGE;
    c = myFgetc(test_stream);
    EXPECT_EQ(1, fgetc_led_fake.call_count);

    ts.device_instance = LED_YELLOW;
    c = myFgetc(test_stream);
    EXPECT_EQ(2, fgetc_led_fake.call_count);

    ts.device_instance = LED_GREEN;
    c = myFgetc(test_stream);
    EXPECT_EQ(3, fgetc_led_fake.call_count);

    ts.device_instance = LED_BLUE;
    c = myFgetc(test_stream);
    EXPECT_EQ(4, fgetc_led_fake.call_count);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}