#ifndef DATE_H
#define DATE_H

#include <sys/time.h>

#define estrm stderr

#define NUM_MONTHS_IN_YEAR 12
#define AFTER 1
#define BEFORE -1
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
    int days;
    int hours;
    int minutes;
    int seconds;
    int microseconds;
};
typedef struct DecomposedTimeval DecomposedTimeval;

struct YearPlusDays {
    int year;
    int remaining_days;
};
typedef struct YearPlusDays YearPlusDays;

struct MonthPlusDays {
    enum months_in_year month;
    int remaining_days;
};
typedef struct MonthPlusDays MonthPlusDays;


int is_leap_year(int year);
int *months_in_year(void);
CalendarDate *create_base_calendar_date(void);
CalendarDate *compute_calendar_date(struct timeval *tvp);
DecomposedTimeval *decompose_timeval(struct timeval *tvp);
int relation_to_base_date(struct timeval *tv);
int num_days_in_feb(int year);
int num_days_in_year(int year);
enum months_in_year next_month(enum months_in_year current_month);
CalendarDate *compute_year_month_day(int days_since_epoch_start);
MonthPlusDays *month_plus_remaining_days(int current_year,
            enum months_in_year start_month, int days_beyond);
YearPlusDays *year_plus_remaining_days(int start_year, int days_beyond);
const char *month_string(enum months_in_year month);
char *format_calendar_date(CalendarDate *cd);
time_t timezone_shift(struct timeval *tvp, struct timezone *tzp);

#endif
