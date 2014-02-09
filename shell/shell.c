/*
 * shell
 */
 #include "shell.h"
 #include "../include/constants.h"
 #include "../util/util.h"
 #include <stdio.h>
 #include <stdlib.h>




/* functions */
/*
 * cmd_exit
 * Purpose:
 * "exit" will exit from the shell (i.e., cause the shell to terminate)
 * by calling the exit system call.
 *
 * Parameters:
 * None
 *
 * Returns:
 * None
 *
 * Side-Effects:
 * Terminates program. Sets exit status code.
 */
 int cmd_exit(int argc, char *argv[]) {
    exit(0);
 }


void startup(void) {
    CommandEntry commands[] = {{"exit", cmd_exit}};
/*
} commands[] = {{"date", cmd_date},
                {"echo", cmd_echo},
                {"exit", cmd_exit},
                {"help", cmd_help}};
*/
}

void print_prompt(FILE *ostrm) {
    fputc('$', ostrm);
    fputc(' ', ostrm);
}

char *create_input_buffer() {
    return (char *) emalloc(sizeof (char) * (MAX_INPUT_LEN + 1),
        "create_input_buffer", stderr);
}

char *read_input(FILE *istrm) {
    char *buf = create_input_buffer();
    buf = fgets(buf, MAX_INPUT_LEN + 1, istrm);
    return buf;
}

CommandLine *parse_input(char *buf) {
    CommandLine *cl;
    cl = (CommandLine *) emalloc(sizeof(CommandLine),
        "parse_input", stderr);
    int num_args = count_args(buf);
    cl->argc = num_args;
    cl->argv = create_argv(num_args);
    char *cp = buf;
    int i = 0;
    while (i < num_args) {
        cp = extract_next_token(cp, cl, i++);
    }
    return cl;
}

int count_args(char *buf) {
    char *cp = buf;
    int num_args = 0;
    int in_arg = 0;
    while (*cp != '\0' && *cp != '\n') {
        if (*cp == ' ' || *cp == '\t') {
            in_arg = 0;
        } else if (!in_arg) {
            in_arg = 1;
            num_args++;
        }
        cp++;
    }
    return num_args;
}

char **create_argv(int num_args) {
    return (char **) emalloc(num_args * sizeof (char *),
        "create_argv", stderr);
}

char *extract_next_token(char *start, CommandLine *cl, int idx) {
    int token_length = 0;
    char *cp;
    /* trim leading whitespace */
    while (*start == ' ' || *start == '\t') {
        start++;
    }
    token_length = measure_token(start);
    cl->argv[idx] = (char *) emalloc(token_length * sizeof(char) + 1,
        "extract_next_token", stderr);
    /* copy over each letter in this token */
    copy_chars(start, cl->argv[idx], token_length);
    cp = start + token_length;
    return cp;
}

int measure_token(char *start) {
    char *cp = start;
    int token_length = 0;
    while (*cp != '\0' && *cp != '\n' &&
            *cp != ' ' && *cp != '\t') {
        token_length++;
        cp++;
    }
    return token_length;
}

void copy_chars(char *start, char *dest, int num_chars) {
    int i;
    char *cp = start;
    for (i = 0; i < num_chars; i++) {
        *dest++ = *cp++;
    }
    *dest = '\0';
}

