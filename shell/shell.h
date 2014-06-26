#ifndef SHELL_H
#define SHELL_H

#include "../include/constants.h"
#include "../include/io.h"

#ifndef K70
#include <sys/time.h>
#else
#include "../time/time.h"
#endif

#ifdef TEST_SHELL
#include <stdio.h>
#endif

/* CONSOLE_IO should be 1 if using the CodeWarrior console IO */
#define CONSOLE_IO 0

#define HELP_TEXT "Shell Commands:\r\n" \
"help                  output this guide\r\n" \
"echo [value ...]      output the specificied whitespace separated string values\r\n" \
"date                  output the current date and time in the current timezone\r\n" \
"setdate [seconds]     set the current date and time to this many seconds since Jan 1, 1900 00:00:00\r\n" \
"settimer [count]      schedule an interrupt to fire in count 1/46875 second increments\r\n" \
"malloc [size]         allocate size bytes of memory and output its address\r\n" \
"free [address]        deallocate memory previously allocated by malloc\r\n" \
"memorymap             print the outstanding addresses of free and used memory\r\n" \
"create [/dev/fs/...]  create a new file-system file with the given name\r\n" \
"delete [/dev/fs/...]  delete a file-system file with the given name\r\n" \
"fopen [/dev/x/y]      open a file for reading and writing\r\n" \
"fclose [stream ID]    close the stream with the given stream ID\r\n" \
"fgetc [stream ID]     read the next character from the given stream ID and print it\r\n" \
"fputc [stream ID] [c] write the character c to the given stream ID\r\n" \
"ser2lcd               continuously copy characters from serial input to LCD. End on Control-D\r\n" \
"touch2led             continuously copy from each touch sensor to the corresponding LED. End on all four touched.\r\n" \
"pot2ser               continuously output the value of the analog potentiomemter to the serial device. End on dial turned min value.\r\n" \
"therm2ser             continuously output the value of the thermistor to the serial device. End when SW1 depressed.\r\n" \
"pb2led                continuously copy from SW1 to orange LED and SW2 to yellow LED. End when both SW1 and SW2 are depressed.\r\n" \
"exit                  terminate this shell\r\n" \



/* in production use only stdin, stdout, and stderr as I/O streams. */
/* in test use dynamic variable input and output streams */
#ifndef TEST_SHELL
#define ostrm STDOUT
#define istrm STDIN
#define estrm STDERR
#endif


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


/* function declarations */

#ifdef TEST_SHELL
int run_shell(FILE *istrm, FILE *ostrm);
int cmd_echo(int argc, char *argv[], FILE *ostrm);
int cmd_exit(int argc, char *argv[], FILE *ostrm);
int cmd_help(int argc, char *argv[], FILE *ostrm);
int cmd_malloc(int argc, char *argv[], FILE *ostrm);
int cmd_free(int argc, char *argv[], FILE *ostrm);
int cmd_memorymap(int argc, char *argv[], FILE *ostrm);
int cmd_date(int argc, char *argv[], FILE *ostrm);
int cmd_setdate(int argc, char *argv[], FILE *ostrm);
#else
int run_shell(void);
int cmd_echo(int argc, char *argv[]);
int cmd_exit(int argc, char *argv[]);
int cmd_help(int argc, char *argv[]);
int cmd_malloc(int argc, char *argv[]);
int cmd_free(int argc, char *argv[]);
int cmd_memorymap(int argc, char *argv[]);
int cmd_date(int argc, char *argv[]);
int cmd_setdate(int argc, char *argv[]);
#endif


/* internal functions */
#ifdef TEST_SHELL
void initialize_shell(FILE *ostrm);
void print_prompt(FILE *output);
char *read_input(FILE *istrm);
char *gets_interactively(char *str, int size, FILE *stream);
#else
void initialize_shell(void);
void print_prompt(void);
char *read_input(void);
#endif

char *create_input_buffer(void);
int count_args(char *buf);
char **create_argv(int num_args);
int measure_token(char *start);
char *advance_past_whitespace(char *start);
char *next_token(char *start, int token_length);
CommandLine *parse_input(char *buf);
CommandLine *create_command_line(int num_args);
CommandEntry *find_command(char *cmd, CommandEntry *cmd_list);
int execute(CommandEntry *ce, int argc, char **argv);

#endif
