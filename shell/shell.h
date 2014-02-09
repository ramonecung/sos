#include <stdio.h>

#ifndef SHELL_H
#define SHELL_H

#define NUM_COMMANDS 4

struct CommandLine {
    int argc;
    char **argv;
};
typedef struct CommandLine CommandLine;





/* supported commands */
struct CommandEntry {
    char *name;
    int (*functionp)(int argc, char *argv[]);
};
typedef struct CommandEntry CommandEntry;


/* function declarations */
int cmd_exit(int argc, char *argv[]);
/*
int cmd_date(int argc, char *argv[]);
int cmd_echo(int argc, char *argv[]);
int cmd_help(int argc, char *argv[]);
*/


void startup(void);

void print_prompt(FILE *ostrm);

char *create_input_buffer();

char *read_input(FILE *istrm);

int count_args(char *buf);

char **create_argv(int num_args);
char *extract_next_token(char *start, CommandLine *cl, int idx);
int measure_token(char *start);
void copy_chars(char *start, char *dest, int num_chars);

CommandLine *parse_input(char *buf);

#endif
