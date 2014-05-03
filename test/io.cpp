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

TEST_F(IOTest, MyFopenButton) {
    test_stream = myFopen("/dev/button/sw1");
    EXPECT_EQ(BUTTON_SW1, test_stream->device_instance);
    EXPECT_EQ(NULL, test_stream->next);
    EXPECT_GE(0, test_stream->stream_id);
    test_stream = myFopen("/dev/button/sw2");
    EXPECT_EQ(BUTTON_SW2, test_stream->device_instance);
}

TEST_F(IOTest, MyFopenLed) {
    test_stream = myFopen("/dev/led/orange");
    EXPECT_EQ(LED_ORANGE, test_stream->device_instance);
    EXPECT_EQ(NULL, test_stream->next);
    EXPECT_GE(0, test_stream->stream_id);

    test_stream = myFopen("/dev/led/yellow");
    EXPECT_EQ(LED_YELLOW, test_stream->device_instance);

    test_stream = myFopen("/dev/led/green");
    EXPECT_EQ(LED_GREEN, test_stream->device_instance);

    test_stream = myFopen("/dev/led/blue");
    EXPECT_EQ(LED_BLUE, test_stream->device_instance);
}

TEST_F(IOTest, MyFopenFileSystem) {
    NamedFile file = {"/dev/fs/data", 0, NULL, NULL};
    find_file_fake.return_val = &file;
    test_stream = myFopen("/dev/fs/data");
    EXPECT_EQ(FILE_SYSTEM, test_stream->device_instance);
}

TEST_F(IOTest, MyFclose) {
    test_stream = myFopen("/dev/button/sw1");
    EXPECT_EQ(SUCCESS, myFclose(test_stream));

    test_stream = myFopen("/dev/led/orange");
    EXPECT_EQ(SUCCESS, myFclose(test_stream));
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
    NamedFile file = {"/dev/fs/data", 0, NULL, NULL};
    find_file_fake.return_val = &file;

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

