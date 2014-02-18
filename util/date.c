#include <sys/time.h>
#include <stdlib.h>
#include "../util/date.h"
#include "../util/util.h"


time_t timezone_shift(struct timeval *tvp, struct timezone *tzp) {
    int min_west = tzp->tz_minuteswest;
    if (tzp->tz_dsttime) {
        min_west -= 60;
    }
    time_t seconds_adjustment = min_west * 60;
    return tvp->tv_sec - seconds_adjustment;
}

CalendarDate *compute_calendar_date(struct timeval *tvp) {
    /* TODO: support pre 1970? */
    int relation = relation_to_base_date(tvp);
    if (relation == BEFORE) {
        efputs("Error: date prior to Jan 1, 1970 00:00\n", estrm);
        return NULL;
    }
    DecomposedTimeval *dtv = decompose_timeval(tvp);
    CalendarDate *cd = compute_year_month_day(dtv->days);
    cd->hour = dtv->hours;
    cd->min = dtv->minutes;
    cd->sec = dtv->seconds;
    cd->usec = dtv->microseconds;
    free(dtv);
    return cd;
}

int relation_to_base_date(struct timeval *tv) {
    if (tv->tv_sec < 0) {
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
    YearPlusDays *ypd = emalloc(sizeof(YearPlusDays),
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
    int *days_array = days_in_month();
    enum months_in_year current_month = start_month;
    int num_days;

    days_array[FEB] = num_days_in_feb(current_year);
    while(1) {
        num_days = days_array[current_month];
        if (remaining_days < num_days) {
            break;
        } else {
            current_month = next_month(current_month);
            remaining_days -= num_days;
        }
    }
    free(days_array);
    mpd->month = current_month;
    mpd->remaining_days = remaining_days;
    return mpd;
}

int *days_in_month(void) {
    int *days_in_month = (int *) emalloc(NUM_MONTHS_IN_YEAR * sizeof(int),
        "days_in_month", estrm);
    days_in_month[JAN] = 31;
    /* as a base assume feb length in non-leap year */
    /* caller may change as needed */
    days_in_month[FEB] = 28;
    days_in_month[MAR] = 31;
    days_in_month[APR] = 30;
    days_in_month[MAY] = 31;
    days_in_month[JUN] = 30;
    days_in_month[JUL] = 31;
    days_in_month[AUG] = 31;
    days_in_month[SEP] = 30;
    days_in_month[OCT] = 31;
    days_in_month[NOV] = 30;
    days_in_month[DEC] = 31;
    return days_in_month;
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
