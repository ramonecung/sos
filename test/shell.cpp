
#include "gtest/gtest.h"

extern "C" {
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "../shell/shell.h"
#include "../util/util.h"
#include "../include/constants.h"
}

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

    free(cl->argv);

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

    free(cl->argv);
    free(cl);
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
    CommandEntry *commands = supported_commands();
    CommandEntry *ce;
    char cmd[] = "exit";
    ce = find_command(cmd, commands);
    EXPECT_STREQ("exit", ce->name);

    char cmd2[] = "sandwich";
    ce = find_command(cmd2, commands);
    EXPECT_EQ(NULL, ce);
}

TEST_F(ShellTest, StringsEqual) {
    int res;
    char str1[] = "dog";
    char str2[] = "dog";
    char str3[] = "cat";
    char str4[] = "doggone";

    res = strings_equal(str1, str2);
    EXPECT_EQ(1, res);
    res = strings_equal(str1, str3);
    EXPECT_EQ(0, res);
    res = strings_equal(str1, str4);
    EXPECT_EQ(0, res);
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

    EXPECT_STREQ("this is a string\n", cp);
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
    EXPECT_EQ(WRITE_ERROR, res);
}

TEST_F(ShellTest, CmdHelp) {
    char help_text[] = HELP_TEXT;
    int size = 256;
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



TEST_F(ShellTest, CmdExitError) {
    int res;
    int argc = 2;
    const char *args[] = {"exit", "1"};
    char **argv = new_array_of_strings(argc, args);
    res = cmd_exit(argc, argv, ostrm);
    EXPECT_EQ(EXIT_CODE_UNSUPPORTED, res);
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
