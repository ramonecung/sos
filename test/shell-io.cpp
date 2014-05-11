#include "gtest/gtest.h"

extern "C" {
#include "../shell/shell.h"
#include "../util/util.h"
#include "../shell/shell-io.h"
#include "../freescaleK70/io.h"
#include "../init/init.h"
}

#include <stdio.h>
#include "../third-party/fff.h"
DEFINE_FFF_GLOBALS;
FAKE_VOID_FUNC(initialize_system);
FAKE_VOID_FUNC(initialize_io);
FAKE_VALUE_FUNC(int, svc_myCreate, const char *);
FAKE_VALUE_FUNC(int, svc_myDelete, const char *);
FAKE_VALUE_FUNC(Stream *, svc_myFopen, const char *);
FAKE_VALUE_FUNC(Stream *, find_stream, unsigned int);
FAKE_VALUE_FUNC(int, svc_myFclose, Stream *);
FAKE_VALUE_FUNC(int, svc_myFgetc, Stream *);
FAKE_VALUE_FUNC(int, svc_myFputc, int, Stream *);
FAKE_VALUE_FUNC(void *, svc_myMalloc, unsigned int);
FAKE_VOID_FUNC(svc_myFree, void *);
FAKE_VALUE_FUNC(int, stream_is_led, Stream *);
FAKE_VALUE_FUNC(int, stream_is_button, Stream *);

class ShellIOTest : public ::testing::Test {
    protected:

  int fds[2];
  FILE *ostrm;
  FILE *istrm;

  Stream *test_stream;
  Stream ts;

  static const int output_string_length = 256;
  char output_string[output_string_length];
  char *output_ptr;

  // You can remove any or all of the following functions if its body
  // is empty.


  ShellIOTest() {
    // You can do set-up work for each test here.
  }

  virtual ~ShellIOTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
    RESET_FAKE(initialize_system);
    RESET_FAKE(initialize_io);
    RESET_FAKE(svc_myCreate);
    RESET_FAKE(svc_myDelete);
    RESET_FAKE(svc_myFopen);
    RESET_FAKE(find_stream);
    RESET_FAKE(svc_myFclose);
    RESET_FAKE(svc_myFgetc);
    RESET_FAKE(svc_myFputc);
    RESET_FAKE(svc_myMalloc);
    RESET_FAKE(svc_myFree);
    RESET_FAKE(stream_is_led);
    RESET_FAKE(stream_is_button);

    FFF_RESET_HISTORY();

    test_stream = &ts;

    output_string[0] = '\0';

    initialize_shell(stdout);
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }

  void OpenStreams(void) {
    pipe(fds);
    ostrm = fdopen(fds[1], "w");
    istrm = fdopen(fds[0], "r");
  }

  char *SendInput(const char *input) {
    OpenStreams();
    fputs(input, ostrm);
    fclose(ostrm);

    char *buf = read_input(istrm);
    fclose(istrm);
    return buf;
  }
};

TEST_F(ShellIOTest, Create) {
    int result;
    int argc = 2;
    const char *args[] = {"create", "/dev/fs/data"};
    char **argv = new_array_of_strings(argc, args);

    OpenStreams();
    result = cmd_create(argc, argv, ostrm);
    delete_array_of_strings(argc, argv);
    fclose(ostrm);
    fclose(istrm);

    EXPECT_EQ(SUCCESS, result);
    EXPECT_EQ(1, svc_myCreate_fake.call_count);
}

TEST_F(ShellIOTest, CreateError) {
    int argc = 2;
    const char *args[] = {"create", "invalid_filename"};
    char **argv = new_array_of_strings(argc, args);

    OpenStreams();
    svc_myCreate_fake.return_val = CANNOT_CREATE_FILE;
    cmd_create(argc, argv, ostrm);
    fclose(ostrm);
    delete_array_of_strings(argc, argv);
    fgets(output_string, output_string_length, istrm);
    fclose(istrm);

    EXPECT_STREQ("create: error creating file\r\n", output_string);
}

TEST_F(ShellIOTest, Fopen) {
    int result;
    int argc = 2;
    const char *args[] = {"fopen", "/dev/fs/data"};
    char **argv = new_array_of_strings(argc, args);

    OpenStreams();

    test_stream->device_instance = FILE_SYSTEM;
    test_stream->stream_id = 1;
    svc_myFopen_fake.return_val = test_stream;
    result = cmd_fopen(argc, argv, ostrm);
    fclose(ostrm);
    delete_array_of_strings(argc, argv);
    fgets(output_string, output_string_length, istrm);

    EXPECT_EQ(SUCCESS, result);
    EXPECT_EQ(1, svc_myFopen_fake.call_count);
    EXPECT_STREQ("file opened with stream ID: 1\r\n", output_string);
}

TEST_F(ShellIOTest, FopenError) {
    int result;
    int argc = 2;
    const char *args[] = {"fopen", "/dev/fs/data"};
    char **argv = new_array_of_strings(argc, args);

    OpenStreams();
    svc_myFopen_fake.return_val = NULL;
    result = cmd_fopen(argc, argv, ostrm);
    fclose(ostrm);
    delete_array_of_strings(argc, argv);
    fgets(output_string, output_string_length, istrm);
    fclose(istrm);

    EXPECT_EQ(CANNOT_OPEN_FILE, result);
    EXPECT_EQ(1, svc_myFopen_fake.call_count);
    EXPECT_STREQ("fopen: error opening file\r\n", output_string);
}

TEST_F(ShellIOTest, Fclose) {
    int result;
    int argc = 2;
    char stream_to_close[8];
    sprintf(stream_to_close, "%d", LED_ORANGE);
    const char *args[] = {"fclose", stream_to_close};

    OpenStreams();
    char **argv = new_array_of_strings(argc, args);
    test_stream->device_instance = LED_ORANGE;
    find_stream_fake.return_val = test_stream;

    result = cmd_fclose(argc, argv, ostrm);
    fclose(ostrm);
    delete_array_of_strings(argc, argv);
    fclose(istrm);
    EXPECT_EQ(SUCCESS, result);
    EXPECT_EQ(1, svc_myFclose_fake.call_count);
    EXPECT_EQ(1, find_stream_fake.call_count);
}

TEST_F(ShellIOTest, FcloseInvalidStream) {
    int result;
    int argc = 2;
    const char *args[] = {"fclose", "-1"};

    OpenStreams();
    char **argv = new_array_of_strings(argc, args);

    result = cmd_fclose(argc, argv, ostrm);
    EXPECT_EQ(CANNOT_CLOSE_FILE, result);

    EXPECT_EQ(0, svc_myFclose_fake.call_count);

    fclose(ostrm);
    delete_array_of_strings(argc, argv);
    fclose(istrm);
}

TEST_F(ShellIOTest, FcloseNullStream) {
    int result;
    int argc = 2;
    const char *args[] = {"fclose", "1"}; /* conceivably valid stream ID */

    OpenStreams();
    char **argv = new_array_of_strings(argc, args);
    find_stream_fake.return_val = NULL;

    result = cmd_fclose(argc, argv, ostrm);
    fclose(ostrm);
    delete_array_of_strings(argc, argv);
    fclose(istrm);
    EXPECT_EQ(CANNOT_CLOSE_FILE, result);
    EXPECT_EQ(0, svc_myFclose_fake.call_count);
    EXPECT_EQ(1, find_stream_fake.call_count);
}

TEST_F(ShellIOTest, Delete) {
    int result;
    int argc = 2;
    const char *args[] = {"delete", "/dev/fs/data"};

    OpenStreams();
    char **argv = new_array_of_strings(argc, args);

    result = cmd_delete(argc, argv, ostrm);
    EXPECT_EQ(SUCCESS, result);
    fclose(ostrm);
    delete_array_of_strings(argc, argv);
    fclose(istrm);
    EXPECT_EQ(1, svc_myDelete_fake.call_count);
}

TEST_F(ShellIOTest, DeleteNonExistentFile) {
    int result;
    int argc = 2;
    const char *args[] = {"delete", "/dev/fs/data"};

    OpenStreams();
    char **argv = new_array_of_strings(argc, args);

    svc_myDelete_fake.return_val = CANNOT_DELETE_FILE;
    result = cmd_delete(argc, argv, ostrm);
    EXPECT_EQ(CANNOT_DELETE_FILE, result);

    fclose(ostrm);
    delete_array_of_strings(argc, argv);
    fgets(output_string, output_string_length, istrm);
    fclose(istrm);

    EXPECT_EQ(1, svc_myDelete_fake.call_count);
    EXPECT_STREQ("delete: cannot delete file\r\n", output_string);
}

TEST_F(ShellIOTest, Fgetc) {
    int result;
    int argc = 2;
    const char *args[] = {"fgetc", "1"}; /* assume 1 is an open stream ID */
    find_stream_fake.return_val = test_stream;

    OpenStreams();
    char **argv = new_array_of_strings(argc, args);

    result = cmd_fgetc(argc, argv, ostrm);
    EXPECT_EQ(SUCCESS, result);
    fclose(ostrm);
    delete_array_of_strings(argc, argv);
    fclose(istrm);
    EXPECT_EQ(1, svc_myFgetc_fake.call_count);
}

TEST_F(ShellIOTest, FgetcNullStream) {
    int result;
    int argc = 2;
    const char *args[] = {"fgetc", "-1"}; /* not a possible open stream ID */

    OpenStreams();
    char **argv = new_array_of_strings(argc, args);

    find_stream_fake.return_val = NULL;
    result = cmd_fgetc(argc, argv, ostrm);
    EXPECT_EQ(CANNOT_GET_CHAR, result);
    fclose(ostrm);
    delete_array_of_strings(argc, argv);
    fgets(output_string, output_string_length, istrm);
    fclose(istrm);
    EXPECT_EQ(0, svc_myFgetc_fake.call_count);
    EXPECT_STREQ("fgetc: cannot get char from null stream\r\n", output_string);
}

TEST_F(ShellIOTest, Fputc) {
    int result;
    int argc = 3;
    const char *args[] = {"fputc", "1", "c"};

    OpenStreams();
    char **argv = new_array_of_strings(argc, args);

    svc_myFputc_fake.return_val = 'c';
    find_stream_fake.return_val = test_stream;
    result = cmd_fputc(argc, argv, ostrm);
    EXPECT_EQ(SUCCESS, result);
    fclose(ostrm);
    delete_array_of_strings(argc, argv);
    fclose(istrm);
    EXPECT_EQ(1, svc_myFputc_fake.call_count);
    EXPECT_EQ('c', svc_myFputc_fake.arg0_history[0]);
}

TEST_F(ShellIOTest, FputcWrongNumberArgs) {
    int result;
    int argc = 2;
    const char *args[] = {"fputc", "1"};

    OpenStreams();
    char **argv = new_array_of_strings(argc, args);

    result = cmd_fputc(argc, argv, ostrm);
    EXPECT_EQ(WRONG_NUMBER_ARGS, result);
    fclose(ostrm);
    delete_array_of_strings(argc, argv);
    fgets(output_string, output_string_length, istrm);
    fclose(istrm);
    EXPECT_STREQ("usage: fputc streamID char\r\n", output_string);
    EXPECT_EQ(0, svc_myFputc_fake.call_count);
}

TEST_F(ShellIOTest, FputcInvalidChar) {
    int result;
    int argc = 3;
    const char *args[] = {"fputc", "1", "dog"};

    OpenStreams();
    char **argv = new_array_of_strings(argc, args);

    result = cmd_fputc(argc, argv, ostrm);
    EXPECT_EQ(INVALID_INPUT, result);
    fclose(ostrm);
    delete_array_of_strings(argc, argv);
    fgets(output_string, output_string_length, istrm);
    fclose(istrm);
    EXPECT_STREQ("fputc: invalid char\r\n", output_string);
    EXPECT_EQ(0, svc_myFputc_fake.call_count);
}

TEST_F(ShellIOTest, FputcNullStream) {
    int result;
    int argc = 3;
    const char *args[] = {"fputc", "-1", "c"};

    OpenStreams();
    char **argv = new_array_of_strings(argc, args);

    find_stream_fake.return_val = NULL;
    result = cmd_fputc(argc, argv, ostrm);
    EXPECT_EQ(CANNOT_PUT_CHAR, result);
    fclose(ostrm);
    delete_array_of_strings(argc, argv);
    fgets(output_string, output_string_length, istrm);
    fclose(istrm);
    EXPECT_STREQ("fputc: cannot put char\r\n", output_string);
    EXPECT_EQ(0, svc_myFputc_fake.call_count);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
