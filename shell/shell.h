#include "../include/constants.h"

#if defined __linux__ || defined __APPLE__|| defined _WIN32 || defined _WIN64
#include <sys/time.h>
#endif

#include <stdio.h>


#ifndef SHELL_H
#define SHELL_H

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


/* function declarations */

#ifdef TEST_SHELL
void run_shell(FILE *istrm, FILE *ostrm);
int cmd_echo(int argc, char *argv[], FILE *ostrm);
int cmd_exit(int argc, char *argv[], FILE *ostrm);
int cmd_help(int argc, char *argv[], FILE *ostrm);
int cmd_malloc(int argc, char *argv[], FILE *ostrm);
int cmd_free(int argc, char *argv[], FILE *ostrm);
int cmd_memorymap(int argc, char *argv[], FILE *ostrm);

#if defined __linux__ || defined __APPLE__|| defined _WIN32 || defined _WIN64
int cmd_date(int argc, char *argv[], FILE *ostrm);
#endif

#else
void run_shell(void);
int cmd_echo(int argc, char *argv[]);
int cmd_exit(int argc, char *argv[]);
int cmd_help(int argc, char *argv[]);
int cmd_malloc(int argc, char *argv[]);
int cmd_free(int argc, char *argv[]);
int cmd_memorymap(int argc, char *argv[]);

#if defined __linux__ || defined __APPLE__|| defined _WIN32 || defined _WIN64
int cmd_date(int argc, char *argv[]);
#endif

#endif


/* internal functions */
void initialize_shell(void);
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
int execute(CommandEntry *ce, int argc, char **argv);

#endif
