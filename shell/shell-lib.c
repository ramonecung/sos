/*
 * shell library
 */
#include "shell.h"
#include "shell-io.h"
#include "../include/constants.h"
#include "../util/util.h"
#include "../util/strings.h"
#include "../memory/memory.h"

#include <stdio.h>
#include <stdlib.h>

#if defined __linux__ || defined __APPLE__|| defined _WIN32 || defined _WIN64
#include "../util/date.h"
#include <sys/time.h>
#endif



/* data */
static CommandEntry commands[] = {{"echo", cmd_echo},
               {"exit", cmd_exit},
               {"help", cmd_help},
               {"malloc", cmd_malloc},
               {"free", cmd_free},
               {"memorymap", cmd_memorymap},
               {"create", cmd_create},
               {"delete", cmd_delete},
               {"fopen", cmd_fopen},
               {"fclose", cmd_fclose},
               {"fgetc", cmd_fgetc},
               {"fputc", cmd_fputc},
#if defined __linux__ || defined __APPLE__|| defined _WIN32 || defined _WIN64
               {"date", cmd_date},
#endif
               {"sentinel", NULL}};


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

    initialize_shell();
    while(1) {
        print_prompt(ostrm);
        input_buffer = read_input(istrm);
        cl = parse_input(input_buffer);
        efree(input_buffer);
        if (cl->argc > 0) {
            ce = find_command(cl->argv[0], commands);
            if (ce == NULL) {
                ce = find_command("help", commands);
            }
            result = execute(ce, cl->argc, cl->argv);
            delete_array_of_strings(cl->argc, cl->argv);
        }
        efree(cl);
    }
}

void initialize_shell(void) {
    void *start_address;
    /* obtain chunk of memory from system for myMalloc and myFree */
    start_address = emalloc(TOTAL_SPACE, "run_shell", estrm);
    initialize_memory(start_address, TOTAL_SPACE);
}

/* shell builtin commands */
/*
 * cmd_exit
 * Purpose:
 *  exit from the shell (i.e., cause the shell to terminate)
 *  by calling the exit system call.
 *
 * Parameters:
 *  argc - number of arguments on the command line including the initial "exit"
 *  argv - array of strings containing the ordered command line arguments
 *
 * Returns:
 *  None
 *
 * Side-Effects:
 *  Terminates program. Sets exit status code.
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

/*
 * cmd_echo
 * Purpose:
 *  output each of the arguments in argv
 *  (except for the first) to stdout with a single space character
 *  between the srguments.
 *
 * Parameters:
 *  argc - number of arguments on the command line including the initial "echo"
 *  argv - array of strings containing the ordered command line arguments
 *
 * Returns:
 *  0 if successful, otherwise an error code
 *
 * Side-Effects:
 *  None
 */
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

/*
 * cmd_help
 * Purpose:
 *  output to stdout a brief
 *  description of the commands accepted by the shell.
 *
 * Parameters:
 *  argc - number of arguments on the command line including the initial "help"
 *  argv - array of strings containing the ordered command line arguments
 *
 * Returns:
 *  0 if successful, otherwise an error code
 *
 * Side-Effects:
 *  None
 */
#ifdef TEST_SHELL
int cmd_help(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_help(int argc, char *argv[]) {
#endif
    const char *ht = HELP_TEXT;
    int res = efputs(ht, ostrm);
    if (res != SUCCESS) {
        return WRITE_ERROR;
    }
    res = efputc('\0', ostrm);
    if (res != SUCCESS) {
        return WRITE_ERROR;
    }
    return SUCCESS;
 }

/*
 * cmd_date
 * Purpose:
 *  output to stdout the current date and time in the current timezone
 *  in the format "January 23, 2014 15:57:07.123456".
 *
 * Parameters:
 *  argc - number of arguments on the command line including the initial "date"
 *  argv - array of strings containing the ordered command line arguments
 *
 * Returns:
 *  0 if successful, otherwise an error code
 *
 * Side-Effects:
 *  None
 */
#if defined __linux__ || defined __APPLE__|| defined _WIN32 || defined _WIN64

#ifdef TEST_SHELL
int cmd_date(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_date(int argc, char *argv[]) {
#endif
    CalendarDate *cd;
    struct timeval tv;
    struct timezone tz;
    int res;
    if (argc > 1) {
        return DATE_ARGS_UNSUPPORTED;
    }
    res = gettimeofday(&tv, &tz);
    if (res < 0) {
        return TIME_ERROR;
    }
    /* tv is in UTC - shift into the current timezone */
    tv.tv_sec = timezone_shift(&tv, &tz);
    cd = compute_calendar_date(&tv);
    if (cd == NULL) {
        return TIME_ERROR;
    }
    char *date_string = format_calendar_date(cd);
    res = efputs(date_string, ostrm);
    if (res != SUCCESS) {
        return res;
    }
    efree(date_string);
    res = efputc('\n', ostrm);
    if (res != SUCCESS) {
        return res;
    }
    efree(cd);
    return SUCCESS;
}

#endif

/*
 * cmd_malloc
 * Purpose:
 *  allocate memory and return its address
 *
 * Parameters:
 *  argc - number of arguments on the command line including the initial "malloc"
 *  argv - array of strings containing the ordered command line arguments
 *
 * Returns:
 *  0 if successful, otherwise an error code
 *
 * Side-Effects:
 *  Allocates heap storage
 */
#ifdef TEST_SHELL
int cmd_malloc(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_malloc(int argc, char *argv[]) {
#endif
    int res;
    void *addr;
    char *endptr = "";
    unsigned long long int num_bytes;

    if (argc != 2) {
        return WRONG_NUMBER_ARGS;
    }

    num_bytes = (unsigned long long int) strtoull(argv[1], &endptr, 0);
    if (*endptr != '\0') {
        res = efputs("malloc: invalid size\n", ostrm);
        if (res != SUCCESS) {
            return res;
        }
        return INVALID_INPUT;
    }

    addr = myMalloc(num_bytes);
    if (addr == NULL) {
        res = efputs("malloc: could not allocate memory\n", ostrm);
        if (res != SUCCESS) {
            return res;
        }
        return MALLOC_ERROR;
    } else {
        fprintf(ostrm, "%p\n", addr);
    }
    return SUCCESS;
 }


/*
 * cmd_free
 * Purpose:
 *  free a previously allocated memory region given its address
 *
 * Parameters:
 *  argc - number of arguments on the command line including the initial "free"
 *  argv - array of strings containing the ordered command line arguments
 *
 * Returns:
 *  0 if successful, otherwise an error code
 *
 * Side-Effects:
 *  Deallocates heap storage
 */
#ifdef TEST_SHELL
int cmd_free(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_free(int argc, char *argv[]) {
#endif
    int res;
    unsigned long long int addr;
    char *endptr = "";

    if (argc != 2) {
        return WRONG_NUMBER_ARGS;
    }

    addr = strtoull(argv[1], &endptr, 0);
    if (*endptr != '\0') {
        res = efputs("free: invalid address\n", ostrm);
        if (res != SUCCESS) {
            return res;
        }
        return INVALID_INPUT;
    }

    myFree((void *) addr);
    res = efputs("free: possibly deallocated memory at given address\n", ostrm);
    return res;
 }

/*
 * cmd_memorymap
 * Purpose:
 *  output the map of both allocated and free memory regions
 *
 * Parameters:
 *  argc - number of arguments on the command line including the initial "memorymap"
 *  argv - array of strings containing the ordered command line arguments
 *
 * Returns:
 *  0 if successful, otherwise an error code
 *
 * Side-Effects:
 *  None
 */
#ifdef TEST_SHELL
int cmd_memorymap(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_memorymap(int argc, char *argv[]) {
#endif
    memoryMap();
    return SUCCESS;
 }

/* shell computation */
CommandEntry *find_command(char *cmd, CommandEntry *cmd_list) {
    int i = 0;
    while (!(strings_equal(cmd_list[i].name, "sentinel"))) {
        if (strings_equal(cmd_list[i].name, cmd)) {
            return &cmd_list[i];
        }
        i++;
    }
    return NULL;
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






/* shell command line interface */
void print_prompt(FILE *ostrm) {
    efputs("$ ", ostrm);
}

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
