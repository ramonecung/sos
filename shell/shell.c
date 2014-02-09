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
    CommandEntry *commands = supported_commands();
}

CommandEntry *supported_commands(void) {
    CommandEntry *commands = emalloc(NUM_COMMANDS * sizeof(CommandEntry),
        "supported_commands", stderr);

    commands[0] = (CommandEntry) {"exit", cmd_exit};
    return commands;
/*
} commands[] = {{"date", cmd_date},
                {"echo", cmd_echo},
                {"exit", cmd_exit},
                {"help", cmd_help}};
*/
}

/* shell computation */
CommandEntry *find_command(char *cmd, CommandEntry *cmd_list) {
    int i = 0;
    while (i < 1) {
        if (strings_equal(cmd_list[i].name, cmd)) {
            return &cmd_list[i];
        }
        i++;
    }
    return NULL;
}

int strings_equal(char *str1, char *str2) {
    char *p1, *p2;
    p1 = str1, p2 = str2;
    while(*p1 && *p2) {
        if (*p1++ != *p2++) {
            return 0;
        }
    }
    if ((*p1 && !*p2) || (!*p1 && *p2)) {
        return 0;
    }
    return 1;
}

/* shell output */
void print_prompt(FILE *ostrm) {
    fputc('$', ostrm);
    fputc(' ', ostrm);
}


/* shell input */
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
    char *cp = buf;
    int num_args = count_args(buf);
    CommandLine *cl = create_command_line(num_args);
    int token_length;
    int i = 0;
    while (i < num_args) {
        cp = advance_past_whitespace(cp);
        token_length = measure_token(cp);
        cl->argv[i++] = next_token(cp, token_length);
        cp += token_length;
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

CommandLine *create_command_line(int num_args) {
    CommandLine *cl = (CommandLine *) emalloc(sizeof(CommandLine),
        "create_command_line", stderr);
    cl->argc = num_args;
    cl->argv = create_argv(num_args);
    return cl;
}

char **create_argv(int num_args) {
    return (char **) emalloc(num_args * sizeof (char *),
        "create_argv", stderr);
}

char *advance_past_whitespace(char *start) {
    while (*start == ' ' || *start == '\t') {
        start++;
    }
    return start;
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

char *next_token(char *start, int token_length) {
    char *dest = (char *) emalloc(token_length * sizeof(char) + 1,
        "next_token", stderr);
    /* copy over each letter in this token */
    char *cp = dest;
    int i;
    for (i = 0; i < token_length; i++) {
        *cp++ = *start++;
    }
    *cp = '\0';
    return dest;
}
