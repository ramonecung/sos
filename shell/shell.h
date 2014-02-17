#include <stdio.h>
#include <sys/time.h>
#include "../include/constants.h"

#ifndef SHELL_H
#define SHELL_H

#define NUM_COMMANDS 4

#define HELP_TEXT "Shell Commands:\n" \
"date               output the current date and time in the current timezone\n" \
"echo [value ...]   output the specificied whitespace separated string values\n" \
"exit               terminate this shell\n" \
"help               output this guide\n"


/* in production use only stdin, stdout, and stderr as I/O streams. */
/* in test use dynamic variable input and output streams */
#ifndef TEST_SHELL
#define ostrm stdout
#define istrm stdin
#endif
#define estrm stderr


/* data structures */
struct CommandLine {
    int argc;
    char **argv;
};
typedef struct CommandLine CommandLine;

/* supported commands */
struct CommandEntry {
    const char *name;
    #ifdef TEST_SHELL
    int (*functionp)(int argc, char *argv[], FILE *ostrm);
    #else
    int (*functionp)(int argc, char *argv[]);
    #endif
};
typedef struct CommandEntry CommandEntry;


struct CalendarDate {
    int year;
    enum months_in_year month;
    int day;
    int hour;
    int min;
    int sec;
    int usec;
};
typedef struct CalendarDate CalendarDate;

struct DecomposedTimeval {
    int days;
    int hours;
    int minutes;
    int seconds;
    int microseconds;
};
typedef struct DecomposedTimeval DecomposedTimeval;

struct YearPlusDays {
    int year;
    int remaining_days;
};
typedef struct YearPlusDays YearPlusDays;

struct MonthPlusDays {
    enum months_in_year month;
    int remaining_days;
};
typedef struct MonthPlusDays MonthPlusDays;

/* function declarations */

#ifdef TEST_SHELL
int cmd_echo(int argc, char *argv[], FILE *ostrm);
int cmd_exit(int argc, char *argv[], FILE *ostrm);
int cmd_help(int argc, char *argv[], FILE *ostrm);
int cmd_date(int argc, char *argv[], FILE *ostrm);
#else
int cmd_echo(int argc, char *argv[]);
int cmd_exit(int argc, char *argv[]);
int cmd_help(int argc, char *argv[]);
int cmd_date(int argc, char *argv[]);
#endif



#ifdef TEST_SHELL
void run_shell(FILE *istrm, FILE *ostrm);
#else
void run_shell(void);
#endif

void print_prompt(FILE *ostrm);

char *create_input_buffer();

char *read_input(FILE *istrm);

int count_args(char *buf);

char **create_argv(int num_args);
int measure_token(char *start);
char *advance_past_whitespace(char *start);
char *next_token(char *start, int token_length);

CommandLine *parse_input(char *buf);
CommandLine *create_command_line(int num_args);


CommandEntry *find_command(char *cmd, CommandEntry *cmd_list);
int strings_equal(const char *str1, char *str2);

int execute(CommandEntry *ce, int argc, char **argv);

int efputc(int c, FILE *stream);

int efputs(const char * s, FILE *stream);



int is_leap_year(int year);
int *months_in_year(void);
CalendarDate *create_base_calendar_date(void);
CalendarDate *compute_calendar_date(struct timeval *tvp);
DecomposedTimeval *decompose_timeval(struct timeval *tvp);
int relation_to_base_date(struct timeval *tv);
int num_days_in_feb(int year);
int num_days_in_year(int year);
enum months_in_year next_month(enum months_in_year current_month);
CalendarDate *compute_year_month_day(int days_since_epoch_start);
MonthPlusDays *month_plus_remaining_days(int current_year,
            enum months_in_year start_month, int days_beyond);
YearPlusDays *year_plus_remaining_days(int start_year, int days_beyond);

#endif
