#include <stdio.h>

struct CommandLine {
    int argc;
    char **argv;
};
typedef struct CommandLine CommandLine;

/* function declarations */

void print_prompt(FILE *ostrm);

char *create_input_buffer();

char *read_input(FILE *istrm);

int count_args(char *buf);

char **create_argv(int num_args);
char *extract_next_token(char *start, CommandLine *cl, int idx);
int measure_token(char *start);
void copy_chars(char *start, char *dest, int num_chars);

CommandLine *parse_input(char *buf);
