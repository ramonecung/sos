#ifndef DATE_H
#define DATE_H

#ifndef K70
#include <sys/time.h>
#else
#include "../freescaleK70/time.h"
#endif

#ifdef TEST_SHELL
#define estrm stderr
#endif

#define NUM_MONTHS_IN_YEAR 12
#define NUM_SECONDS_IN_DAY 86400
#define NUM_SECONDS_IN_HOUR 3600
#define NUM_SECONDS_IN_MINUTE 60
#ifndef K70
#define EPOCH_START_YEAR 1970
#else
#define EPOCH_START_YEAR 1900
#endif

#ifndef TEST_SHELL
#define ostrm STDOUT
#define istrm STDIN
#define estrm STDERR
#endif

enum months_in_year {
    JAN,
    FEB,
    MAR,
    APR,
    MAY,
    JUN,
    JUL,
    AUG,
    SEP,
    OCT,
    NOV,
    DEC
};

struct CalendarDate {
    int year;
    enum months_in_year month;
    int day;
    int hour;
    int min;
    int sec;
    int usec;
};
typedef struct CalendarDate CalendarDate;

struct DecomposedTimeval {
    int years;
    int months;
    int days;
    int hours;
    int minutes;
    int seconds;
    int microseconds;
};
typedef struct DecomposedTimeval DecomposedTimeval;

struct YearsMonthsDays {
    int years;
    int months;
    int days;
};
typedef struct YearsMonthsDays YearsMonthsDays;


int is_leap_year(int year);
int *days_per_month(void);
CalendarDate *create_base_calendar_date(void);
CalendarDate *compute_calendar_date(struct timeval *tvp);
DecomposedTimeval *decompose_timeval(struct timeval *tvp);
int num_days_in_feb(int year);
int num_days_in_year(int year);
enum months_in_year next_month(enum months_in_year current_month);
YearsMonthsDays *years_months_days(int start_year, int days_beyond);
const char *month_string(enum months_in_year month);
char *format_calendar_date(CalendarDate *cd);
time_t timezone_shift(struct timeval *tvp, struct timezone *tzp);

#endif /* DATE_H */
