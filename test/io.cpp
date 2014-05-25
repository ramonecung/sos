extern "C" {
#include "../include/constants.h"
#include "../memory/memory.h"
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

FAKE_VOID_FUNC(initialize_io_touch_pad);
FAKE_VALUE_FUNC(int, fgetc_touch_pad, Stream *);
FAKE_VALUE_FUNC(int, fputc_touch_pad, int, Stream *);

FAKE_VOID_FUNC(initialize_io_adc);
FAKE_VALUE_FUNC(int, fgetc_potentiometer, Stream *);
FAKE_VALUE_FUNC(int, fputc_potentiometer, int, Stream *);
FAKE_VALUE_FUNC(int, fgetc_thermistor, Stream *);
FAKE_VALUE_FUNC(int, fputc_thermistor, int, Stream *);

FAKE_VOID_FUNC(initialize_io_lcd);
FAKE_VALUE_FUNC(int, fgetc_lcd, Stream *);
FAKE_VALUE_FUNC(int, fputc_lcd, int, Stream *);

FAKE_VOID_FUNC(initialize_io_uart);
FAKE_VALUE_FUNC(int, fgetc_uart, Stream *);
FAKE_VALUE_FUNC(int, fputc_uart, int, Stream *);
FAKE_VOID_FUNC(intSerialIOInit);

FAKE_VOID_FUNC(initialize_io_button);
FAKE_VALUE_FUNC(int, fgetc_button, Stream *);
FAKE_VALUE_FUNC(int, fputc_button, int, Stream *);

FAKE_VOID_FUNC(initialize_io_led);
FAKE_VALUE_FUNC(int, fgetc_led);
FAKE_VALUE_FUNC(int, fputc_led, int, Stream *);

FAKE_VALUE_FUNC(NamedFile *, find_file, const char *);
FAKE_VOID_FUNC(setup_stream_fs, Stream *);

FAKE_VOID_FUNC(initialize_io_fs);
FAKE_VALUE_FUNC(int, create_fs, const char *);
FAKE_VALUE_FUNC(int, delete_fs, const char *);
FAKE_VALUE_FUNC(int, fgetc_fs, Stream *);
FAKE_VALUE_FUNC(int, fputc_fs, int, Stream *);

class IOTest : public ::testing::Test {
  protected:

  // You can remove any or all of the following functions if its body
  // is empty.

  Stream *test_stream;
  Stream ts;

  IOTest() {
    // You can do set-up work for each test here.
    initialize_memory();
    initialize_io();
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

    RESET_FAKE(initialize_io_touch_pad);
    RESET_FAKE(fgetc_touch_pad);
    RESET_FAKE(fputc_touch_pad);

    RESET_FAKE(initialize_io_adc);
    RESET_FAKE(fgetc_potentiometer);
    RESET_FAKE(fputc_potentiometer);
    RESET_FAKE(fgetc_thermistor);
    RESET_FAKE(fputc_thermistor);

    RESET_FAKE(initialize_io_lcd);
    RESET_FAKE(fgetc_lcd);
    RESET_FAKE(fputc_lcd);

    RESET_FAKE(initialize_io_uart);
    RESET_FAKE(fgetc_uart);
    RESET_FAKE(fputc_uart);
    RESET_FAKE(intSerialIOInit);

    RESET_FAKE(initialize_io_button);
    RESET_FAKE(fgetc_button);
    RESET_FAKE(fputc_button);

    RESET_FAKE(initialize_io_led);
    RESET_FAKE(fgetc_led);
    RESET_FAKE(fputc_led);

    RESET_FAKE(find_file);
    RESET_FAKE(setup_stream_fs);

    RESET_FAKE(initialize_io_fs);
    RESET_FAKE(create_fs);
    RESET_FAKE(delete_fs);
    RESET_FAKE(fgetc_fs);
    RESET_FAKE(fputc_fs);

    FFF_RESET_HISTORY();

    test_stream = &ts;
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
};

TEST_F(IOTest, MyFopenUart) {
    int sid;
    sid = next_stream_id();
    test_stream = myFopen("/dev/uart/uart2");
    EXPECT_EQ(UART2, test_stream->device_instance);
    EXPECT_GE(sid + 1, test_stream->stream_id);
}

TEST_F(IOTest, MyFopenButton) {
    int sid;
    sid = next_stream_id();
    test_stream = myFopen("/dev/button/sw1");
    EXPECT_EQ(BUTTON_SW1, test_stream->device_instance);
    EXPECT_GE(sid + 1, test_stream->stream_id);
    test_stream = myFopen("/dev/button/sw2");
    EXPECT_EQ(BUTTON_SW2, test_stream->device_instance);
}

TEST_F(IOTest, MyFopenLed) {
    int sid;
    sid = next_stream_id();
    test_stream = myFopen("/dev/led/orange");
    EXPECT_EQ(LED_ORANGE, test_stream->device_instance);
    EXPECT_GE(sid + 1, test_stream->stream_id);

    test_stream = myFopen("/dev/led/yellow");
    EXPECT_EQ(LED_YELLOW, test_stream->device_instance);

    test_stream = myFopen("/dev/led/green");
    EXPECT_EQ(LED_GREEN, test_stream->device_instance);

    test_stream = myFopen("/dev/led/blue");
    EXPECT_EQ(LED_BLUE, test_stream->device_instance);
}

TEST_F(IOTest, MyFopenFileSystem) {
    NamedFile file = {"/dev/fs/data", 0, NULL, NULL, NULL};

    find_file_fake.return_val = &file;
    test_stream = myFopen("/dev/fs/data");
    EXPECT_EQ(FILE_SYSTEM, test_stream->device_instance);
}

TEST_F(IOTest, MyFclose) {
    Stream *s1, *s2, *s3, *s4;
    NamedFile file = {"/dev/fs/data", 0, NULL, NULL, NULL};


    s1 = myFopen("/dev/uart/uart2");
    s2 = myFopen("/dev/button/sw1");
    s3 = myFopen("/dev/led/orange");

    find_file_fake.return_val = &file;
    s4 = myFopen("/dev/fs/data");

    EXPECT_EQ(SUCCESS, myFclose(s1));
    EXPECT_EQ(SUCCESS, myFclose(s2));
    EXPECT_EQ(SUCCESS, myFclose(s3));
    EXPECT_EQ(SUCCESS, myFclose(s4));
}

TEST_F(IOTest, MyFgetc) {
    int c;

    ts.device_instance = UART2;
    c = myFgetc(test_stream);
    EXPECT_EQ(1, fgetc_uart_fake.call_count);

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

    ts.device_instance = UART2;
    c = myFputc('c', test_stream);
    EXPECT_EQ(1, fputc_uart_fake.call_count);

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
    NamedFile file = {"/dev/fs/data", 0, NULL, NULL, NULL};
    find_file_fake.return_val = &file;

    stream = myFopen("/dev/uart/uart2");
    EXPECT_EQ(stream, find_stream(stream->stream_id));

    stream = myFopen("/dev/led/orange");
    EXPECT_EQ(stream, find_stream(stream->stream_id));

    stream = myFopen("/dev/button/sw1");
    EXPECT_EQ(stream, find_stream(stream->stream_id));

    stream = myFopen("/dev/fs/data");
    EXPECT_EQ(stream, find_stream(stream->stream_id));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

