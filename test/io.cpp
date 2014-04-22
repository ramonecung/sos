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

FAKE_VOID_FUNC(initialize_io_button);
FAKE_VALUE_FUNC(Stream *, fopen_button, enum device_instance);
FAKE_VALUE_FUNC(int, fclose_button, Stream *);
FAKE_VALUE_FUNC(int, fgetc_button, Stream *);
FAKE_VALUE_FUNC(int, fputc_button, int, Stream *);
FAKE_VALUE_FUNC(Stream *, find_stream_button, enum device_instance);

FAKE_VOID_FUNC(initialize_io_led);
FAKE_VALUE_FUNC(Stream *, fopen_led, enum device_instance);
FAKE_VALUE_FUNC(int, fclose_led, Stream *);
FAKE_VALUE_FUNC(int, fgetc_led);
FAKE_VALUE_FUNC(int, fputc_led, int, Stream *);
FAKE_VALUE_FUNC(Stream *, find_stream_led, enum device_instance);

FAKE_VOID_FUNC(initialize_io_fs);
FAKE_VALUE_FUNC(int, create_fs, const char *);
FAKE_VALUE_FUNC(int, delete_fs, const char *);
FAKE_VALUE_FUNC(Stream *, fopen_fs, const char *);
FAKE_VALUE_FUNC(int, fclose_fs, Stream *);
FAKE_VALUE_FUNC(int, fgetc_fs, Stream *);
FAKE_VALUE_FUNC(int, fputc_fs, int, Stream *);
FAKE_VALUE_FUNC(int, filename_valid);
FAKE_VALUE_FUNC(Stream *, find_stream_fs, enum device_instance);

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

    RESET_FAKE(initialize_io_button);
    RESET_FAKE(fopen_button);
    RESET_FAKE(fclose_button);
    RESET_FAKE(fgetc_button);
    RESET_FAKE(fputc_button);
    RESET_FAKE(find_stream_button);

    RESET_FAKE(initialize_io_led);
    RESET_FAKE(fopen_led);
    RESET_FAKE(fclose_led);
    RESET_FAKE(fgetc_led);
    RESET_FAKE(fputc_led);
    RESET_FAKE(find_stream_led);

    RESET_FAKE(initialize_io_led);
    RESET_FAKE(create_fs);
    RESET_FAKE(delete_fs);
    RESET_FAKE(fopen_fs);
    RESET_FAKE(fclose_fs);
    RESET_FAKE(filename_valid);
    RESET_FAKE(find_stream_fs);

    FFF_RESET_HISTORY();

    ts.device = &d;
    test_stream = &ts;
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
};

TEST_F(IOTest, MyFopenButton) {
    test_stream = myFopen("/dev/button/sw1");
    EXPECT_EQ(BUTTON_SW1, fopen_button_fake.arg0_history[0]);
    EXPECT_EQ(1, fopen_button_fake.call_count);

    test_stream = myFopen("/dev/button/sw2");
    EXPECT_EQ(BUTTON_SW2, fopen_button_fake.arg0_history[1]);
}

TEST_F(IOTest, MyFopenLed) {
    test_stream = myFopen("/dev/led/orange");
    EXPECT_EQ(LED_ORANGE, fopen_led_fake.arg0_history[0]);

    test_stream = myFopen("/dev/led/yellow");
    EXPECT_EQ(LED_YELLOW, fopen_led_fake.arg0_history[1]);

    test_stream = myFopen("/dev/led/green");
    EXPECT_EQ(LED_GREEN, fopen_led_fake.arg0_history[2]);

    test_stream = myFopen("/dev/led/blue");
    EXPECT_EQ(LED_BLUE, fopen_led_fake.arg0_history[3]);
}

TEST_F(IOTest, MyFopenFileSystem) {
    filename_valid_fake.return_val = 1;
    test_stream = myFopen("/dev/fs/data");
    EXPECT_EQ(1, fopen_fs_fake.call_count);
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

TEST_F(IOTest, MyFputc) {
    int c;

    ts.device_instance = BUTTON_SW1;
    c = myFputc('c', test_stream);
    EXPECT_EQ(1, fputc_button_fake.call_count);

    ts.device_instance = BUTTON_SW2;
    c = myFputc('c', test_stream);
    EXPECT_EQ(2, fputc_button_fake.call_count);

    ts.device_instance = LED_ORANGE;
    c = myFputc('c', test_stream);
    EXPECT_EQ(1, fputc_led_fake.call_count);

    ts.device_instance = LED_YELLOW;
    c = myFputc('c', test_stream);
    EXPECT_EQ(2, fputc_led_fake.call_count);

    ts.device_instance = LED_GREEN;
    c = myFputc('c', test_stream);
    EXPECT_EQ(3, fputc_led_fake.call_count);

    ts.device_instance = LED_BLUE;
    c = myFputc('c', test_stream);
    EXPECT_EQ(4, fputc_led_fake.call_count);
}

TEST_F(IOTest, FindStream) {
    Stream *stream;
    stream = find_stream(LED_ORANGE);
    EXPECT_EQ(1, find_stream_led_fake.call_count);
    EXPECT_EQ(LED_ORANGE, find_stream_led_fake.arg0_history[0]);

    stream = find_stream(BUTTON_SW1);
    EXPECT_EQ(1, find_stream_button_fake.call_count);
    EXPECT_EQ(BUTTON_SW1, find_stream_button_fake.arg0_history[0]);

    stream = find_stream((enum device_instance) FILE_SYSTEM_ID_START);
    EXPECT_EQ(1, find_stream_fs_fake.call_count);
    EXPECT_EQ(FILE_SYSTEM_ID_START, find_stream_fs_fake.arg0_history[0]);

    stream = find_stream((enum device_instance) FILE_SYSTEM_ID_END);
    EXPECT_EQ(2, find_stream_fs_fake.call_count);
    EXPECT_EQ(FILE_SYSTEM_ID_END, find_stream_fs_fake.arg0_history[1]);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

