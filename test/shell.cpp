
#include "gtest/gtest.h"

extern "C" {
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
/* ubuntu doesn't like SIZE_MAX */
#ifndef SIZE_MAX
#define SIZE_MAX 18446744073709551615LLU
#endif

#include "../include/constants.h"
#include "../memory/memory.h"
#include "../shell/shell.h"
#include "../util/util.h"
#include "../freescaleK70/io.h"
#include "../init/init.h"
}

#include "../third-party/fff.h"
DEFINE_FFF_GLOBALS;
FAKE_VOID_FUNC(initialize_system);
FAKE_VOID_FUNC(initialize_io);
FAKE_VALUE_FUNC(int, svc_myCreate, const char *);
FAKE_VALUE_FUNC(int, svc_myDelete, const char *);
FAKE_VALUE_FUNC(Stream *, svc_myFopen, const char *);
FAKE_VALUE_FUNC(int, svc_myFclose, Stream *);
FAKE_VALUE_FUNC(int, svc_myFgetc, Stream *);
FAKE_VALUE_FUNC(int, svc_myFputc, int, Stream *);
FAKE_VALUE_FUNC(int, svc_myFputs, const char *, Stream *);
FAKE_VOID_FUNC(svc_myFflush);
FAKE_VOID_FUNC(svc_setTimer, uint16_t);
FAKE_VALUE_FUNC(void *, svc_myMalloc, unsigned int);
FAKE_VOID_FUNC(svc_myFree, void *);
FAKE_VALUE_FUNC(Stream *, find_stream, unsigned int);
FAKE_VALUE_FUNC(int, stream_is_led, Stream *);
FAKE_VALUE_FUNC(int, stream_is_button, Stream *);
FAKE_VOID_FUNC(delay, unsigned long int);

class ShellTest : public ::testing::Test {
    protected:

  // You can remove any or all of the following functions if its body
  // is empty.

  int fds[2];
  FILE *ostrm;
  FILE *istrm;

  ShellTest() {
    // You can do set-up work for each test here.
  }

  virtual ~ShellTest() {
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
    RESET_FAKE(svc_myFclose);
    RESET_FAKE(find_stream);
    RESET_FAKE(svc_myFputc);
    RESET_FAKE(svc_myFputs);
    RESET_FAKE(svc_myFflush);
    RESET_FAKE(svc_setTimer);
    RESET_FAKE(svc_myMalloc);
    RESET_FAKE(stream_is_led);
    RESET_FAKE(stream_is_button);
    RESET_FAKE(delay);

    FFF_RESET_HISTORY();

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

  void CloseStreams(void) {
    fclose(istrm);
    fclose(ostrm);
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

typedef ShellTest ShellDeathTest;

TEST_F(ShellTest, Prompt) {
    char c, d;
    OpenStreams();
    print_prompt(ostrm);
    fclose(ostrm);

    c = fgetc(istrm);
    d = fgetc(istrm);
    fclose(istrm);

    EXPECT_EQ('$', c);
    EXPECT_EQ(' ', d);
}

TEST_F(ShellTest, ReadInput) {
    const char *input = "cmd_help";
    /* intentionally not using SendInput because this tests read_input */
    OpenStreams();
    fputs(input, ostrm);
    fclose(ostrm);

    char *buf = read_input(istrm);
    fclose(istrm);
    EXPECT_STREQ(input, buf);
}

TEST_F(ShellTest, InputBufferLen) {
    char expected_buf[257];
    char long_buf[512];
    int i;

    for (i = 0; i < 256; i++) {
        expected_buf[i] = 'x';
    }
    expected_buf[256] = '\0';

    for (i = 0; i < 512; i++) {
        long_buf[i] = 'x';
    }

    char *buf = SendInput(long_buf);
    EXPECT_STREQ(expected_buf, buf);
}

TEST_F(ShellTest, InputBufferLine) {
    const char expected_buf[] = "12345\n";
    const char two_line_buf[] = "12345\n6789";
    char *buf = SendInput(two_line_buf);
    EXPECT_STREQ(expected_buf, buf);
}

TEST_F(ShellTest, CountArgs) {
    const char *input = "cmd_help";
    char *buf = SendInput(input);
    int ac = count_args(buf);
    EXPECT_EQ(1, ac);
}

TEST_F(ShellTest, CreateCommandLine) {
    CommandLine *cl = create_command_line(5);
    EXPECT_EQ(5, cl->argc);
}

TEST_F(ShellTest, AdvancePastWhitespace) {
    char input[] = " \t \t cmd_help";
    char *cp = input + 5;
    char *cp2 = advance_past_whitespace(input);
    EXPECT_EQ(cp, cp2);
}

TEST_F(ShellTest, MeasureToken) {
    char input[] = "cmd_help \n";
    int token_length = measure_token(input);
    EXPECT_EQ(8, token_length);
}

TEST_F(ShellTest, NextToken) {
    CommandLine *cl;
    cl = (CommandLine *) emalloc(sizeof(CommandLine),
        "ExtractNextToken", stderr);
    int num_args;
    char *cp;
    int i;

    num_args = 1;
    cl->argc = num_args;
    cl->argv = create_argv(num_args);

    char input[] = " cmd_help \n";
    cp = input;
    int token_length;
    i = 0;
    while (i < num_args) {
        cp = advance_past_whitespace(cp);
        token_length = measure_token(cp);
        cl->argv[i++] = next_token(cp, token_length);
    }
    EXPECT_STREQ("cmd_help", cl->argv[0]);

    efree(cl->argv);

    num_args = 3;
    cl->argc = num_args;
    cl->argv = create_argv(num_args);

    char input2[] = " a1 a2\ta3\t";
    cp = input2;
    i = 0;
    while (i < num_args) {
        cp = advance_past_whitespace(cp);
        token_length = measure_token(cp);
        cl->argv[i++] = next_token(cp, token_length);
        cp = cp + token_length;
    }
    EXPECT_STREQ("a1", cl->argv[0]);
    EXPECT_STREQ("a2", cl->argv[1]);
    EXPECT_STREQ("a3", cl->argv[2]);

    efree(cl->argv);
    efree(cl);
}

TEST_F(ShellTest, ParseNullInput) {
    CommandLine *cl;
    const char *input = "\n";
    char *buf = SendInput(input);
    cl = parse_input(buf);
    EXPECT_EQ(0, cl->argc);
}

TEST_F(ShellTest, ParseOneArg) {
    CommandLine *cl;
    const char *input = " cmd_help ";
    char *buf = SendInput(input);
    cl = parse_input(buf);
    EXPECT_EQ(1, cl->argc);
    EXPECT_STREQ("cmd_help", cl->argv[0]);
}

TEST_F(ShellTest, ParseTwoArgs) {
    CommandLine *cl;
    const char *input = "echo hello";
    char *buf = SendInput(input);
    cl = parse_input(buf);
    EXPECT_EQ(2, cl->argc);
    EXPECT_STREQ("echo", cl->argv[0]);
    EXPECT_STREQ("hello", cl->argv[1]);
}

TEST_F(ShellTest, ParseManyArgs) {
    CommandLine *cl;
    const char *input = "echo     this  is some     input   string";
    char *buf = SendInput(input);
    cl = parse_input(buf);
    EXPECT_EQ(6, cl->argc);
    EXPECT_STREQ("is", cl->argv[2]);
    EXPECT_STREQ("string", cl->argv[5]);
}

TEST_F(ShellTest, FindCommand) {
    CommandEntry commands[] = {{"date", cmd_date},
               {"echo", cmd_echo},
               {"exit", cmd_exit},
               {"help", cmd_help},
               {"sentinel", NULL}};
    CommandEntry *ce;
    char cmd[] = "exit";
    ce = find_command(cmd, commands);
    EXPECT_STREQ("exit", ce->name);

    char cmd2[] = "sandwich";
    ce = find_command(cmd2, commands);
    EXPECT_EQ(NULL, ce);
}



TEST_F(ShellTest, CmdEcho) {
    int size = 32;
    char str[size];
    char *cp;
    int argc = 5;
    const char *args[] = {"echo", "this", "is", "a", "string"};
    char **argv = new_array_of_strings(argc, args);

    OpenStreams();
    cmd_echo(argc, argv, ostrm);
    fclose(ostrm);
    delete_array_of_strings(argc, argv);
    cp = fgets(str, size, istrm);
    fclose(istrm);

    EXPECT_STREQ("this is a string\r\n", cp);
}

TEST_F(ShellTest, CmdEchoError) {
    int argc = 5;
    const char *args[] = {"echo", "this", "is", "a", "string"};
    char **argv = new_array_of_strings(argc, args);
    int res;
    /* open the output stream for reading, which should error on write */
    FILE *strm = fopen("/dev/null", "r");
    res = cmd_echo(argc, argv, strm);
    fclose(strm);
    delete_array_of_strings(argc, argv);
    EXPECT_EQ(WRITE_ERROR, res);
}

TEST_F(ShellTest, CmdHelp) {
    char help_text[] = HELP_TEXT;
    int size = 2048;
    char str[size];
    char c, *cp;
    int argc = 1;
    const char *args[] = {"help"};
    char **argv = new_array_of_strings(argc, args);

    OpenStreams();
    cmd_help(argc, argv, ostrm);
    fclose(ostrm);
    delete_array_of_strings(argc, argv);
    cp = str;
    while ((c = fgetc(istrm)) != EOF) {
        *cp++ = c;
    }
    fclose(istrm);
    EXPECT_STREQ(help_text, str);
}

TEST_F(ShellTest, CmdDate) {
    int res;
    int argc = 1;
    const char *args[] = {"date"};
    char **argv = new_array_of_strings(argc, args);

    OpenStreams();
    res = cmd_date(argc, argv, ostrm);
    fclose(ostrm);
    fclose(istrm);
    delete_array_of_strings(argc, argv);
    EXPECT_EQ(SUCCESS, res);
}

TEST_F(ShellTest, CmdDateError) {
    int res;
    int argc = 2;
    const char *args[] = {"date", "-u"};
    char **argv = new_array_of_strings(argc, args);
    res = cmd_date(argc, argv, ostrm);
    delete_array_of_strings(argc, argv);
    EXPECT_EQ(DATE_ARGS_UNSUPPORTED, res);
}

/*
If exactly one argument is needed, the shell needs to guarantee that only
one argument -- no more and no less -- is specified.
*/
TEST_F(ShellTest, CmdMallocInputError) {
    int res;
    int argc = 1;
    const char *args[] = {"malloc"};
    char **argv = new_array_of_strings(argc, args);
    res = cmd_malloc(argc, argv, ostrm);
    delete_array_of_strings(argc, argv);
    EXPECT_EQ(WRONG_NUMBER_ARGS, res);
}

TEST_F(ShellTest, CmdMallocInvalidSize) {
    int res;
    int size = 40;
    char str[size];
    char *cp;

    int argc = 2;
    const char *args[] = {"malloc", "cat"};
    char **argv = new_array_of_strings(argc, args);

    OpenStreams();
    res = cmd_malloc(argc, argv, ostrm);
    EXPECT_EQ(INVALID_INPUT, res);
    fclose(ostrm);
    delete_array_of_strings(argc, argv);
    cp = fgets(str, size, istrm);
    fclose(istrm);
    EXPECT_STREQ("malloc: invalid size\r\n", cp);
}

/*
If the call is not
successful, it should output an appropriate error message to stderr.
*/
TEST_F(ShellTest, CmdMallocAllocationError) {
    int res;
    int size = 40;
    char str[size];
    char *cp;

    int argc = 2;
    char number[32];
    /* if using real malloc:
    sprintf(number, "%llu", SIZE_MAX);
     */
    sprintf(number, "%u", TOTAL_SPACE);
    const char *args[] = {"malloc", number};
    char **argv = new_array_of_strings(argc, args);

    OpenStreams();
    svc_myMalloc_fake.return_val = (void *) NULL;
    res = cmd_malloc(argc, argv, ostrm);
    EXPECT_EQ(MALLOC_ERROR, res);
    fclose(ostrm);
    delete_array_of_strings(argc, argv);
    cp = fgets(str, size, istrm);
    fclose(istrm);
    EXPECT_STREQ("malloc: could not allocate memory\r\n", cp);
}

TEST_F(ShellTest, CmdMallocPrintsAddress) {
    int res;
    int size = 40;
    char str[size];
    char *cp;

    int argc = 2;
    char number[32];
    sprintf(number, "%u", 1024);
    const char *args[] = {"malloc", number};
    char **argv = new_array_of_strings(argc, args);

    OpenStreams();
    svc_myMalloc_fake.return_val = (void *) 0x123;
    res = cmd_malloc(argc, argv, ostrm);
    EXPECT_EQ(SUCCESS, res);
    fclose(ostrm);
    delete_array_of_strings(argc, argv);
    cp = fgets(str, size, istrm);
    fclose(istrm);
    EXPECT_STREQ("0x123\r\n", cp);
}


TEST_F(ShellTest, CmdFreeInputError) {
    int res;
    int argc = 1;
    const char *args[] = {"free"};
    char **argv = new_array_of_strings(argc, args);
    res = cmd_free(argc, argv, ostrm);
    delete_array_of_strings(argc, argv);
    EXPECT_EQ(WRONG_NUMBER_ARGS, res);
}

TEST_F(ShellTest, CmdFreeInvalidAddress) {
    int res;
    int size = 40;
    char str[size];
    char *cp;

    int argc = 2;
    const char *args[] = {"free", "cat"};
    char **argv = new_array_of_strings(argc, args);

    OpenStreams();
    res = cmd_free(argc, argv, ostrm);
    EXPECT_EQ(INVALID_INPUT, res);
    fclose(ostrm);
    delete_array_of_strings(argc, argv);
    cp = fgets(str, size, istrm);
    fclose(istrm);
    EXPECT_STREQ("free: invalid address\r\n", cp);
}

TEST_F(ShellTest, CmdFreeNormalOutput) {
    int res;
    int size = 64;
    char str[size];
    char *cp;

    int argc = 2;
    char address[32];

    void *addr = emalloc(8, "CmdFreeNormalOutput", stderr);
    sprintf(address, "%p", addr);
    const char *args[] = {"free", address};
    char **argv = new_array_of_strings(argc, args);

    OpenStreams();
    res = cmd_free(argc, argv, ostrm);
    EXPECT_EQ(SUCCESS, res);
    fclose(ostrm);
    delete_array_of_strings(argc, argv);
    cp = fgets(str, size, istrm);
    fclose(istrm);
    EXPECT_STREQ("free: possibly deallocated memory at given address\r\n", cp);
}

/*
The hexadecimal address
should be output with a prefix of 0x followed by the hexadecimal
constant followed by a newline character.
*/

TEST_F(ShellTest, CmdExitError) {
    int res;
    int argc = 2;
    const char *args[] = {"exit", "1"};
    char **argv = new_array_of_strings(argc, args);
    res = cmd_exit(argc, argv, ostrm);
    delete_array_of_strings(argc, argv);
    EXPECT_EQ(EXIT_ARGS_UNSUPPORTED, res);
}


TEST_F(ShellDeathTest, ExecuteExitCommand) {
    int ac = 1;
    char **av = create_argv(ac);
    char input[] = "exit";
    CommandEntry ce = (CommandEntry) {input, cmd_exit};
    EXPECT_EXIT(execute(&ce, ac, av), ::testing::ExitedWithCode(0), "");
}

TEST_F(ShellDeathTest, CmdExit) {
    int ac = 1;
    char input[] = "exit";
    char **av = create_argv(ac);
    av[0] = input;
    EXPECT_EXIT(cmd_exit(ac, av, ostrm), ::testing::ExitedWithCode(0), "");
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
