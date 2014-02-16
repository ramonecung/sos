/*
 * shell
 */
 #include "shell.h"
 #include "../include/constants.h"
 #include "../include/constants.h"
 #include "../util/util.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <sys/time.h>


/* file data */
static CommandEntry commands[] = {{"date", cmd_date},
               {"echo", cmd_echo},
               {"exit", cmd_exit},
               {"help", cmd_help}};


/* main run loop */
#ifdef TEST_SHELL
void run_shell(FILE *istrm, FILE *ostrm) {
#else
void run_shell(void) {
#endif
    char *input_buffer;
    CommandLine *cl;
    CommandEntry *ce;
    int result;
    while(1) {
        print_prompt(ostrm);
        input_buffer = read_input(istrm);
        cl = parse_input(input_buffer);
        if (cl->argc == 0) {
            continue;
        }
        ce = find_command(cl->argv[0], commands);
        if (ce == NULL) {
            ce = find_command("help", commands);
        }
        result = execute(ce, cl->argc, cl->argv);
        delete_array_of_strings(cl->argc, cl->argv);
    }
}

/* shell builtin commands */
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
#ifdef TEST_SHELL
int cmd_exit(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_exit(int argc, char *argv[]) {
#endif
    int exit_status = 0;
    if (argc > 1) {
        return EXIT_ARGS_UNSUPPORTED;
    }
    exit(exit_status);
 }

#ifdef TEST_SHELL
int cmd_echo(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_echo(int argc, char *argv[]) {
#endif
    int i, j, res;
    if (argc > 1) {
        /* print a space after all but the last item */
        j = argc - 1;
        for (i = 1; i < j; i++) {
            res = efputs(argv[i], ostrm);
            if (res != SUCCESS) {
                return res;
            }
            res = efputc(' ', ostrm);
            if (res != SUCCESS) {
                return res;
            }
        }
        res = efputs(argv[j], ostrm);
        if (res != SUCCESS) {
            return res;
        }
    }
    res = efputc('\n', ostrm);
    if (res != SUCCESS) {
        return res;
    }
    return res;
}

#ifdef TEST_SHELL
int cmd_help(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_help(int argc, char *argv[]) {
#endif
    const char *ht = HELP_TEXT;
    return efputs(ht, ostrm);
 }

#ifdef TEST_SHELL
int cmd_date(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_date(int argc, char *argv[]) {
#endif
    struct timeval tv;
    int res;
    if (argc > 1) {
        return DATE_ARGS_UNSUPPORTED;
    }
    res = gettimeofday(&tv, NULL);
    if (res < 0) {
        return TIME_ERROR;
    }
    format_time(&tv);
    return SUCCESS;
}


/* shell computation */
CommandEntry *find_command(char *cmd, CommandEntry *cmd_list) {
    int i = 0;
    while (i < NUM_COMMANDS) {
        if (strings_equal(cmd_list[i].name, cmd)) {
            return &cmd_list[i];
        }
        i++;
    }
    return NULL;
}

int strings_equal(const char *base_s, char *new_s) {
    const char *p1;
    char *p2;
    p1 = base_s, p2 = new_s;
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

int execute(CommandEntry *ce, int argc, char **argv) {
    int res;
    #ifdef TEST_SHELL
    int (*fp)(int argc, char *argv[], FILE *ostrm) = ce->functionp;
    res = fp(argc, argv, estrm);
    #else
    int (*fp)(int argc, char *argv[]) = ce->functionp;
    res = fp(argc, argv);
    #endif
    if (res != SUCCESS) {
        efputc((char) res, estrm);
    }
    return res;
}

void format_time(struct timeval *tvp) {
    time_t sec = tvp->tv_sec;
    suseconds_t usec = tvp->tv_usec;
}



/* shell output */
void print_prompt(FILE *ostrm) {
    int res;
    res = efputs("$ ", ostrm);
}

int efputc(int c, FILE *stream) {
    int rv = fputc(c, stream);
    if (rv == EOF) {
        return WRITE_ERROR;
    } else {
        return SUCCESS;
    }
}

int efputs(const char *s, FILE *stream) {
    int rv = fputs(s, stream);
    if (rv == EOF) {
        return WRITE_ERROR;
    } else {
        return SUCCESS;
    }
}

/* shell input */
char *create_input_buffer() {
    return (char *) emalloc(sizeof (char) * (MAX_INPUT_LEN + 1),
        "create_input_buffer", estrm);
}

char *read_input(FILE *istrm) {
    char *buf = create_input_buffer();
    buf = fgets(buf, MAX_INPUT_LEN + 1, istrm);
    /* in unix buf being NULL means either error or EOF */
    /* will we ever see EOF? if so this check is invalid */
    if (buf == NULL) {
        efputc((char) READ_ERROR, estrm);
    }
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
        "create_command_line", estrm);
    cl->argc = num_args;
    cl->argv = create_argv(num_args);
    return cl;
}

char **create_argv(int num_args) {
    return (char **) emalloc(num_args * sizeof (char *),
        "create_argv", estrm);
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
        "next_token", estrm);
    /* copy over each letter in this token */
    char *cp = dest;
    int i;
    for (i = 0; i < token_length; i++) {
        *cp++ = *start++;
    }
    *cp = '\0';
    return dest;
}
