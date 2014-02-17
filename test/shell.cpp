
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
    CommandEntry commands[] = {{"date", cmd_date},
                   {"echo", cmd_echo},
                   {"exit", cmd_exit},
                   {"help", cmd_help}};
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

TEST_F(ShellTest, CmdDate) {
    int res;
    int argc = 1;
    const char *args[] = {"date"};
    char **argv = new_array_of_strings(argc, args);

    OpenStreams();
    res = cmd_date(argc, argv, ostrm);
    fclose(ostrm);
    fclose(istrm);
    EXPECT_EQ(SUCCESS, res);
}

TEST_F(ShellTest, FormatCalendarDate) {
    CalendarDate *cd;
    char *date_string;
    struct timeval tv;

    cd = create_base_calendar_date();
    date_string = format_calendar_date(cd);
    EXPECT_STREQ("January 1, 1970 00:00:00.000000", date_string);

    tv.tv_sec = 1392606296;
    tv.tv_usec = 123456;
    cd = compute_calendar_date(&tv);
    date_string = format_calendar_date(cd);
    EXPECT_STREQ("February 17, 2014 03:04:56.123456", date_string);
}

TEST_F(ShellTest, MonthString) {

    EXPECT_STREQ("January", month_string(JAN));
    EXPECT_STREQ("May", month_string(MAY));
    EXPECT_STREQ("December", month_string(DEC));
}

TEST_F(ShellTest, YearPlusRemainingDays) {
    int start_year;
    int days_beyond;
    YearPlusDays *ypd;

    start_year = 1970;
    days_beyond = 0;
    ypd = year_plus_remaining_days(start_year, days_beyond);
    EXPECT_EQ(1970, ypd->year);
    EXPECT_EQ(0, ypd->remaining_days);

    start_year = 1970;
    days_beyond = 365;
    ypd = year_plus_remaining_days(start_year, days_beyond);
    EXPECT_EQ(1971, ypd->year);
    EXPECT_EQ(0, ypd->remaining_days);

    start_year = 1970;
    days_beyond = 365 + 365 + 366; /* 1970, 1971, 1972 */
    ypd = year_plus_remaining_days(start_year, days_beyond);
    EXPECT_EQ(1973, ypd->year);
    EXPECT_EQ(0, ypd->remaining_days);

    start_year = 1970;
    days_beyond = 365 + 365 + 366 + 364;
    ypd = year_plus_remaining_days(start_year, days_beyond);
    EXPECT_EQ(1973, ypd->year);
    EXPECT_EQ(364, ypd->remaining_days);

    start_year = 2012;
    days_beyond = 366 + 365 + 3; /* 2012, 2013 and three days */
    ypd = year_plus_remaining_days(start_year, days_beyond);
    EXPECT_EQ(2014, ypd->year);
    EXPECT_EQ(3, ypd->remaining_days);

    free(ypd);
}

TEST_F(ShellTest, MonthPlusDays) {
    int current_year;
    enum months_in_year start_month;
    int days_beyond;
    MonthPlusDays *mpd;

    current_year = 1970;
    start_month = JAN;
    days_beyond = 0;
    mpd = month_plus_remaining_days(current_year, start_month, days_beyond);
    EXPECT_EQ(JAN, mpd->month);
    EXPECT_EQ(0, mpd->remaining_days);

    current_year = 1970;
    start_month = JAN;

    days_beyond = 3;
    mpd = month_plus_remaining_days(current_year, start_month, days_beyond);
    EXPECT_EQ(JAN, mpd->month);
    EXPECT_EQ(3, mpd->remaining_days);

    days_beyond = 31;
    mpd = month_plus_remaining_days(current_year, start_month, days_beyond);
    EXPECT_EQ(FEB, mpd->month);
    EXPECT_EQ(0, mpd->remaining_days);

    days_beyond = 32;
    mpd = month_plus_remaining_days(current_year, start_month, days_beyond);
    EXPECT_EQ(FEB, mpd->month);
    EXPECT_EQ(1, mpd->remaining_days);

    days_beyond = 60;
    mpd = month_plus_remaining_days(current_year, start_month, days_beyond);
    EXPECT_EQ(MAR, mpd->month);
    EXPECT_EQ(1, mpd->remaining_days);

    current_year = 2012;
    start_month = JAN;
    days_beyond = 60;
    mpd = month_plus_remaining_days(current_year, start_month, days_beyond);
    EXPECT_EQ(MAR, mpd->month);
    EXPECT_EQ(0, mpd->remaining_days);

    start_month = FEB;
    days_beyond = 60;
    mpd = month_plus_remaining_days(current_year, start_month, days_beyond);
    EXPECT_EQ(APR, mpd->month);
    EXPECT_EQ(0, mpd->remaining_days);

    free(mpd);
}

TEST_F(ShellTest, MonthsInYear) {
    int *days_per_month = months_in_year();
    EXPECT_EQ(31, days_per_month[JAN]);
    EXPECT_EQ(28, days_per_month[FEB]);
    EXPECT_EQ(30, days_per_month[APR]);
    EXPECT_EQ(31, days_per_month[DEC]);
}

TEST_F(ShellTest, NextMonth) {
    EXPECT_EQ(FEB, next_month(JAN));
    EXPECT_EQ(MAR, next_month(FEB));
    EXPECT_EQ(JAN, next_month(DEC));
}

TEST_F(ShellTest, CreateBaseCalendarDate) {
    CalendarDate *cd = create_base_calendar_date();
    EXPECT_EQ(1970, cd->year);
    EXPECT_EQ(JAN, cd->month);
    EXPECT_EQ(1, cd->day);
    EXPECT_EQ(0, cd->hour);
    EXPECT_EQ(0, cd->min);
    EXPECT_EQ(0, cd->sec);
    EXPECT_EQ(0, cd->usec);
}

TEST_F(ShellTest, IsLeapYear) {
    EXPECT_EQ(1, is_leap_year(2400));
    EXPECT_EQ(1, is_leap_year(2104));
    EXPECT_EQ(0, is_leap_year(2100));
    EXPECT_EQ(1, is_leap_year(2016));
    EXPECT_EQ(0, is_leap_year(2014));
    EXPECT_EQ(1, is_leap_year(2012));
    EXPECT_EQ(1, is_leap_year(2000));
    EXPECT_EQ(1, is_leap_year(1976));
    EXPECT_EQ(0, is_leap_year(1970));
    EXPECT_EQ(1, is_leap_year(1968));
    EXPECT_EQ(0, is_leap_year(1900));
    EXPECT_EQ(0, is_leap_year(1800));
    EXPECT_EQ(1, is_leap_year(1600));
    EXPECT_EQ(0, is_leap_year(300));
    EXPECT_EQ(1, is_leap_year(4));
    EXPECT_EQ(1, is_leap_year(0));
}

TEST_F(ShellTest, RelationToBaseDate) {
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int relation;
    relation = relation_to_base_date(&tv);
    EXPECT_EQ(AFTER, relation);

    tv.tv_sec = -1;
    tv.tv_usec = 999999;
    relation = relation_to_base_date(&tv);
    EXPECT_EQ(BEFORE, relation);
}

TEST_F(ShellTest, ComputeCalendarDateError) {
    struct timeval tv;
    tv.tv_sec = -1;
    tv.tv_usec = 999999;
    EXPECT_EQ(NULL, compute_calendar_date(&tv));
}

TEST_F(ShellTest, DecomposeTimeval) {
    struct timeval tv;
    tv.tv_sec = 1392606296;
    tv.tv_usec = 123456;
    DecomposedTimeval *dtv;
    dtv = decompose_timeval(&tv);
    EXPECT_EQ(16118, dtv->days);
    EXPECT_EQ(3, dtv->hours);
    EXPECT_EQ(4, dtv->minutes);
    EXPECT_EQ(56, dtv->seconds);

    tv.tv_sec = 1392591896;
    dtv = decompose_timeval(&tv);
    EXPECT_EQ(16117, dtv->days);
}

TEST_F(ShellTest, ComputeYearMonthDay) {
    int days_since;
    CalendarDate *cd;
    days_since = 0;
    cd = compute_year_month_day(days_since);
    EXPECT_EQ(1970, cd->year);
    EXPECT_EQ(JAN, cd->month);
    EXPECT_EQ(1, cd->day);

    days_since = 1;
    cd = compute_year_month_day(days_since);
    EXPECT_EQ(1970, cd->year);
    EXPECT_EQ(JAN, cd->month);
    EXPECT_EQ(2, cd->day);

    days_since = 16118;
    cd = compute_year_month_day(days_since);
    EXPECT_EQ(2014, cd->year);
    EXPECT_EQ(FEB, cd->month);
    EXPECT_EQ(17, cd->day);

    days_since = 16131;
    cd = compute_year_month_day(days_since);
    EXPECT_EQ(2014, cd->year);
    EXPECT_EQ(MAR, cd->month);
    EXPECT_EQ(2, cd->day);
}



TEST_F(ShellTest, NumDaysInFeb) {
    int num_days;
    num_days = num_days_in_feb(2012);
    EXPECT_EQ(29, num_days);
    num_days = num_days_in_feb(2014);
    EXPECT_EQ(28, num_days);
}

TEST_F(ShellTest, NumDaysInYear) {
    int num_days;
    num_days = num_days_in_year(2012);
    EXPECT_EQ(366, num_days);
    num_days = num_days_in_year(2014);
    EXPECT_EQ(365, num_days);
}

TEST_F(ShellTest, CmdDateError) {
    int res;
    int argc = 2;
    const char *args[] = {"date", "-u"};
    char **argv = new_array_of_strings(argc, args);
    res = cmd_date(argc, argv, ostrm);
    EXPECT_EQ(DATE_ARGS_UNSUPPORTED, res);
}


TEST_F(ShellTest, CmdExitError) {
    int res;
    int argc = 2;
    const char *args[] = {"exit", "1"};
    char **argv = new_array_of_strings(argc, args);
    res = cmd_exit(argc, argv, ostrm);
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
