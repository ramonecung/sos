#include <stdio.h>

#ifndef SHELL_H
#define SHELL_H

#define NUM_COMMANDS 4

/* in production use only stdin, stdout, and stderr as I/O streams. */
/* in test use dynamic variable input and output streams */
#ifndef TEST_SHELL
#define ostrm stdout
#define istrm stdin
#endif
#define estrm stderr

struct CommandLine {
    int argc;
    char **argv;
};
typedef struct CommandLine CommandLine;

/* supported commands */
struct CommandEntry {
    char *name;
    #ifdef TEST_SHELL
    int (*functionp)(int argc, char *argv[], FILE *ostrm);
    #else
    int (*functionp)(int argc, char *argv[]);
    #endif
};
typedef struct CommandEntry CommandEntry;


/* function declarations */

//int cmd_echo(int argc, char *argv[]);
#ifdef TEST_SHELL
int cmd_echo(int argc, char *argv[], FILE *ostrm);
int cmd_exit(int argc, char *argv[], FILE *ostrm);
#else
int cmd_echo(int argc, char *argv[]);
int cmd_exit(int argc, char *argv[]);
#endif
/*
int cmd_date(int argc, char *argv[]);
int cmd_help(int argc, char *argv[]);
*/


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
//void copy_chars(char *start, char *dest, int num_chars);
char *next_token(char *start, int token_length);

CommandLine *parse_input(char *buf);
CommandLine *create_command_line(int num_args);

CommandEntry *supported_commands(void);

CommandEntry *find_command(char *cmd, CommandEntry *cmd_list);
int strings_equal(char *str1, char *str2);

int execute(CommandEntry *ce, int argc, char **argv);

#endif
