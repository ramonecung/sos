/*
 * shell
 */
 #include "shell.h"
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
    CalendarDate *cd;
    struct timeval tv;
    int res;
    if (argc > 1) {
        return DATE_ARGS_UNSUPPORTED;
    }
    res = gettimeofday(&tv, NULL);
    if (res < 0) {
        return TIME_ERROR;
    }
    /* TODO: result is UTC - shift into the current timezone */
    cd = compute_calendar_date(&tv);
    char *date_string = format_calendar_date(cd);
    res = efputs(date_string, ostrm);
    if (res != SUCCESS) {
        return res;
    }
    res = efputc('\n', ostrm);
    if (res != SUCCESS) {
        return res;
    }
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



CalendarDate *compute_calendar_date(struct timeval *tvp) {
    /* TODO: support pre 1970? */
    int relation = relation_to_base_date(tvp);
    if (relation == BEFORE) {
        efputs("Error: date prior to Jan 1, 1970 00:00 UTC\n", estrm);
        return NULL;
    }
    DecomposedTimeval *dtv = decompose_timeval(tvp);
    CalendarDate *cd = compute_year_month_day(dtv->days);
    cd->hour = dtv->hours;
    cd->min = dtv->minutes;
    cd->sec = dtv->seconds;
    cd->usec = dtv->microseconds;
    return cd;
}

int relation_to_base_date(struct timeval *tv) {
    time_t combined_usec = tv->tv_sec * 1000000;
    combined_usec += tv->tv_usec;
    if (combined_usec < 0) {
        return BEFORE;
    } else {
        return AFTER;
    }
}

DecomposedTimeval *decompose_timeval(struct timeval *tvp) {
    int remaining_seconds = tvp->tv_sec;
    int days;
    int days_in_seconds;
    int hours;
    int hours_in_seconds;
    int minutes;
    int minutes_in_seconds;
    int seconds;

    /* use integer division to truncate */
    days = remaining_seconds / 86400;
    days_in_seconds = days * 86400;
    remaining_seconds = remaining_seconds - days_in_seconds;

    hours = remaining_seconds / 3600;
    hours_in_seconds = hours * 3600;
    remaining_seconds = remaining_seconds - hours_in_seconds;

    minutes = remaining_seconds / 60;
    minutes_in_seconds = minutes * 60;
    seconds = remaining_seconds - minutes_in_seconds;

    DecomposedTimeval *dtv = emalloc(sizeof(DecomposedTimeval),
        "decompose_timeval", estrm);
    dtv->days = days;
    dtv->hours = hours;
    dtv->minutes = minutes;
    dtv->seconds = seconds;
    dtv->microseconds = tvp->tv_usec;
    return dtv;
}

CalendarDate *compute_year_month_day(int days_since_epoch_start) {
    int remaining_days = days_since_epoch_start;
    CalendarDate *cd = create_base_calendar_date();
    YearPlusDays *ypd;
    MonthPlusDays *mpd;

    ypd = year_plus_remaining_days(cd->year, remaining_days);
    cd->year = ypd->year;
    remaining_days = ypd->remaining_days;
    free(ypd);

    mpd = month_plus_remaining_days(cd->year, cd->month, remaining_days);
    cd->month = mpd->month;
    remaining_days = mpd->remaining_days;
    free(mpd);

    cd->day = 1 + remaining_days;
    return cd;
}

CalendarDate *create_base_calendar_date(void) {
    CalendarDate *cd = (CalendarDate *) emalloc(sizeof(CalendarDate),
        "create_calendar_date", estrm);
    cd->year = 1970;
    cd->month = JAN;
    cd->day = 1;
    cd->hour = 0;
    cd->min = 0;
    cd->sec = 0;
    cd->usec= 0;
    return cd;
}

YearPlusDays *year_plus_remaining_days(int start_year, int days_beyond) {
    int remaining_days = days_beyond;
    YearPlusDays *ypd = emalloc(sizeof(DecomposedTimeval),
        "year_plus_remaining_days", estrm);
    int current_year = start_year;
    int days_in_year;
    while(1) {
        days_in_year = num_days_in_year(current_year);
        if (remaining_days < days_in_year) {
            break;
        } else {
            current_year++;
            remaining_days -= days_in_year;
        }
    }
    ypd->year = current_year;
    ypd->remaining_days = remaining_days;
    return ypd;
}

MonthPlusDays *month_plus_remaining_days(int current_year,
            enum months_in_year start_month, int days_beyond) {
    int remaining_days = days_beyond;
    MonthPlusDays *mpd = emalloc(sizeof(MonthPlusDays),
        "month_plus_remaining_days", estrm);
    int *days_in_months = months_in_year();
    enum months_in_year current_month = start_month;
    int days_in_month;

    days_in_months[FEB] = num_days_in_feb(current_year);
    while(1) {
        days_in_month = days_in_months[current_month];
        if (remaining_days < days_in_month) {
            break;
        } else {
            current_month = next_month(current_month);
            remaining_days -= days_in_month;
        }
    }
    mpd->month = current_month;
    mpd->remaining_days = remaining_days;
    return mpd;
}

int *months_in_year(void) {
    int *months_in_year = (int *) emalloc(NUM_MONTHS_IN_YEAR * sizeof(int),
        "months_in_year", estrm);
    months_in_year[JAN] = 31;
    /* as a base assume feb length in non-leap year */
    /* caller may change as needed */
    months_in_year[FEB] = 28;
    months_in_year[MAR] = 31;
    months_in_year[APR] = 30;
    months_in_year[MAY] = 31;
    months_in_year[JUN] = 30;
    months_in_year[JUL] = 31;
    months_in_year[AUG] = 31;
    months_in_year[SEP] = 30;
    months_in_year[OCT] = 31;
    months_in_year[NOV] = 30;
    months_in_year[DEC] = 31;
    return months_in_year;
}

enum months_in_year next_month(enum months_in_year current_month) {
    switch(current_month) {
        case JAN:
            return FEB;
        case FEB:
            return MAR;
        case MAR:
            return APR;
        case APR:
            return MAY;
        case MAY:
            return JUN;
        case JUN:
            return JUL;
        case JUL:
            return AUG;
        case AUG:
            return SEP;
        case SEP:
            return OCT;
        case OCT:
            return NOV;
        case NOV:
            return DEC;
        case DEC:
            return JAN;
        default:
            /* error */
            return -1;
    }
}

int num_days_in_feb(int year) {
    if (is_leap_year(year)) {
        return 29;
    } else {
        return 28;
    }
}

int num_days_in_year(int year) {
    if (is_leap_year(year)) {
        return 366;
    } else {
        return 365;
    }
}

/*
 * Every year that is evenly divisible by
 * four is a leap year, except that every year divisible by 100 is not
 * a leap year, except that every year divisible by 400 is a leap year.
 *
 */
int is_leap_year(int year) {
    if (year % 400 == 0) {
        return 1;
    } else if (year % 100 == 0) {
        return 0;
    } else if (year % 4 == 0) {
        return 1;
    } else {
        return 0;
    }
}

char *format_calendar_date(CalendarDate *cd) {
    /* TODO: make this size precise */
    int size = 128;
    char *str = emalloc(size * sizeof(char), "format_calendar_date", estrm);
    char *cp = str;
    int n;
    n = sprintf(cp, "%s", month_string(cd->month));
    cp += n;
    n = sprintf(cp, " %d, %d ", cd->day, cd->year);
    cp += n;
    n = sprintf(cp, "%02d:%02d:%02d", cd->hour, cd->min, cd->sec);
    cp += n;
    n = sprintf(cp, ".%06d", cd->usec);
    return str;
}

const char *month_string(enum months_in_year month) {
    switch(month) {
        case JAN:
            return "January";
        case FEB:
            return "February";
        case MAR:
            return "March";
        case APR:
            return "April";
        case MAY:
            return "May";
        case JUN:
            return "June";
        case JUL:
            return "July";
        case AUG:
            return "August";
        case SEP:
            return "September";
        case OCT:
            return "October";
        case NOV:
            return "November";
        case DEC:
            return "December";
        default:
            /* error */
            return "";
    }
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
