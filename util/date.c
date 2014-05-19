#ifndef K70
#include <sys/time.h>
#else
#include "../freescaleK70/time.h"
#endif

#include "../include/constants.h"
#include "../freescaleK70/io.h"

#include "../util/date.h"
#include "../util/util.h"
#include "../util/strings.h"

#include <stdio.h>

time_t timezone_shift(struct timeval *tvp, struct timezone *tzp) {
    int min_west = tzp->tz_minuteswest;
    if (tzp->tz_dsttime) {
        min_west -= 60;
    }
    time_t seconds_adjustment = min_west * 60;
    return tvp->tv_sec - seconds_adjustment;
}

CalendarDate *compute_calendar_date(struct timeval *tvp) {
    uint64_t i;
    char buf[64];
    if (tvp->tv_sec < 0) {
        sprintf(buf,  "Error: date prior to January 1, %d 00:00:00\r\n", EPOCH_START_YEAR);
        efputs(buf, estrm);
        return NULL;
    }
    DecomposedTimeval *dtv = decompose_timeval(tvp);
    CalendarDate *cd = create_base_calendar_date();
    cd->year = cd->year + dtv->years;
    cd->month = JAN;
    for (i = 0; i < dtv->months; i++) {
        cd->month = next_month(cd->month);
    }
    cd->day = 1 + dtv->days;
    cd->hour = dtv->hours;
    cd->min = dtv->minutes;
    cd->sec = dtv->seconds;
    cd->usec = dtv->microseconds;
    efree(dtv);
    return cd;
}

CalendarDate *create_base_calendar_date(void) {
    CalendarDate *cd = (CalendarDate *) emalloc(sizeof(CalendarDate),
        "create_calendar_date", estrm);
    cd->year = EPOCH_START_YEAR;
    cd->month = JAN;
    cd->day = 1;
    cd->hour = 0;
    cd->min = 0;
    cd->sec = 0;
    cd->usec= 0;
    return cd;
}

DecomposedTimeval *decompose_timeval(struct timeval *tvp) {
    uint64_t remaining_seconds = tvp->tv_sec;
    uint64_t days;
    uint64_t days_in_seconds;
    uint64_t hours;
    uint64_t hours_in_seconds;
    uint64_t minutes;
    uint64_t minutes_in_seconds;
    uint64_t seconds;
    YearsMonthsDays *ymd;

    /* use integer division to truncate */
    days = remaining_seconds / NUM_SECONDS_IN_DAY;
    days_in_seconds = days * NUM_SECONDS_IN_DAY;
    remaining_seconds = remaining_seconds - days_in_seconds;

    hours = remaining_seconds / NUM_SECONDS_IN_HOUR;
    hours_in_seconds = hours * NUM_SECONDS_IN_HOUR;
    remaining_seconds = remaining_seconds - hours_in_seconds;

    minutes = remaining_seconds / NUM_SECONDS_IN_MINUTE;
    minutes_in_seconds = minutes * NUM_SECONDS_IN_MINUTE;
    seconds = remaining_seconds - minutes_in_seconds;

    ymd = years_months_days(EPOCH_START_YEAR, days);

    DecomposedTimeval *dtv = emalloc(sizeof(DecomposedTimeval),
        "decompose_timeval", estrm);
    dtv->years = ymd->years;
    dtv->months = ymd->months;
    dtv->days = ymd->days;
    dtv->hours = hours;
    dtv->minutes = minutes;
    dtv->seconds = seconds;
    dtv->microseconds = tvp->tv_usec;
    return dtv;
}

YearsMonthsDays *years_months_days(int start_year, int days_beyond) {
    int years = 0;
    int months = 0;
    int remaining_days = days_beyond;
    YearsMonthsDays *ymd = emalloc(sizeof(YearsMonthsDays),
        "years_months_days", estrm);
    int current_year = start_year;
    int days_in_year;
    enum months_in_year current_month = JAN;
    int days_in_month;
    int *days_array = days_per_month();

    while(1) {
        days_in_year = num_days_in_year(current_year);
        if (remaining_days < days_in_year) {
            ymd->years = years;
            break;
        } else {
            years++;
            current_year++;
            remaining_days -= days_in_year;
        }
    }

    days_array[FEB] = num_days_in_feb(current_year);
    while(1) {
        days_in_month = days_array[current_month];
        if (remaining_days < days_in_month) {
            ymd->months = months;
            break;
        } else {
            months++;
            current_month = next_month(current_month);
            remaining_days -= days_in_month;
        }
    }
    efree(days_array);
    ymd->days = remaining_days;
    return ymd;
}

int *days_per_month(void) {
    int *days_per_month = (int *) emalloc(NUM_MONTHS_IN_YEAR * sizeof(int),
        "days_per_month", estrm);
    days_per_month[JAN] = 31;
    /* as a base assume feb length in non-leap year */
    /* caller may change as needed */
    days_per_month[FEB] = 28;
    days_per_month[MAR] = 31;
    days_per_month[APR] = 30;
    days_per_month[MAY] = 31;
    days_per_month[JUN] = 30;
    days_per_month[JUL] = 31;
    days_per_month[AUG] = 31;
    days_per_month[SEP] = 30;
    days_per_month[OCT] = 31;
    days_per_month[NOV] = 30;
    days_per_month[DEC] = 31;
    return days_per_month;
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
    int len_month;
    int len_day;
    int len_year = 4;
    int len_m_d_y_gaps = 1 + 2 + 1; /* space, comma, space, space */
    int len_timestamp = 2 + 1 + 2 + 1 + 2 + 1 + 6; /* 00:00:00.000000 */
    int size;
    const char *month = month_string(cd->month);
    char *str, *cp;
    int n;

    len_month = string_length(month);
    if (cd->day < 10) {
        len_day = 1;
    } else {
        len_day = 2;
    }
    /* room for string and null byte */
    size = len_month + len_day + len_year + len_m_d_y_gaps + len_timestamp + 1;
    str = emalloc(size * sizeof(char), "format_calendar_date", estrm);

    cp = str;
    n = sprintf(cp, "%s", month);
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
