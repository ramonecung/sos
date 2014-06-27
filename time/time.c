#include <stdint.h>
#include "time.h"
#include "../include/constants.h"
#include "../util/util.h"
#include "../arm/critical_section.h"

#define YEAR_1900_SECONDS 0
#define ONE_MILLION_USECONDS 1000000

/*
 * Need to support datetimes from 1900 to 3000 with millisecond precision
 * Given:
 * 1100 years
 * 86,400,000 milliseconds in 1 day
 * Either 365 or 366 days in 1 year
 * Then
 * (86,400,000 * 365 * 1100) < num milliseconds in 1100 years < (86,400,000 * 366 * 1100)
 * 3.46896e+13 < num milliseconds in 1100 years < 3.478464e+13
 * and since
 * 2^44 < 3.46896e+13 < 3.478464e+13 < 2^45 - 1
 * we therefore need at least 45 bits (unsigned) to accommodate the number of milliseconds.
 */
static uint64_t milliseconds_since_epoch = 14400000; /* 14400000 UTC == 0 EDT */

/* TODO: stop hardcoding this to EDT */
struct timezone system_timezone = { 300, 1 };

void disable_interrupts_time(void) {
    disable_interrupts();
}

void enable_interrupts_time(void) {
    enable_interrupts();
}

/* set flextimer to interrupt every millisecond and increment */

/* must disable interrupts when getting or setting milliseconds_since_epoch */
/* it is a 64 bit value, meaning two words are accessed separately */
/* if the time of day interrupt occurs in between the data will be corrupted */

/*
 * this routine contains all actions to be performed when a FlexTimer 0
 * interrupt occurs.
 */
void flexTimer0Action(void) {
    disable_interrupts_time();
    milliseconds_since_epoch++;
    enable_interrupts_time();
}

uint64_t get_current_millis(void) {
    uint64_t rv;
    disable_interrupts_time();
    rv = milliseconds_since_epoch;
    enable_interrupts_time();
    return rv;
}

void set_current_millis(uint64_t millis) {
    disable_interrupts_time();
    milliseconds_since_epoch = millis;
    enable_interrupts_time();
}

/*
 * gettimeofday
 *
 * A 0 return value indicates that the call succeeded.
 * A -1 return value indicates an error occurred
 *
 * NOTE: SOS only maintains millisecond precision
 * The passed in struct timeval * has a microseconds field,
 * and this function populates it with a value rounded down
 * to the nearest thousand-microsecond value.
 */
int gettimeofday(struct timeval *tp, void *tzp) {
    uint64_t total_milliseconds;
    uint64_t remaining_milliseconds;
    if (tp != NULL) {
        total_milliseconds = get_current_millis();
        tp->tv_sec = total_milliseconds / 1000;
        remaining_milliseconds = total_milliseconds - (1000 * tp->tv_sec);
        tp->tv_usec = remaining_milliseconds * 1000;
    }
    if (tzp != NULL) {
        ((struct timezone *) tzp)->tz_minuteswest = system_timezone.tz_minuteswest;
        ((struct timezone *) tzp)->tz_dsttime = system_timezone.tz_dsttime;
    }
    return 0;
}

/*
 * settimeofday
 *
 * The struct timeval to which tp points should have the seconds
 * and microseconds since 1900.
 *
 * Example: The following input will set the date to May 18, 2014 19:15:00
 * tp->tv_sec = 3609443700
 * tp->tv_usec = 0
 * tzp->tz_minuteswest = 300
 * tzp->tz_dsttime = 1
 * Returns:
 * A 0 return value indicates that the call succeeded.
 * A -1 return value indicates an error occurred
 */
int settimeofday(const struct timeval *tp, const struct timezone *tzp) {
    uint64_t total_milliseconds;
    if (tp != NULL) {
        /* basic error checking */
        if (tp->tv_sec < YEAR_1900_SECONDS) {
            return -1;
        }
        if (tp->tv_usec < 0 || tp->tv_usec >= ONE_MILLION_USECONDS) {
            return -1;
        }
        total_milliseconds = tp->tv_sec * 1000;
        total_milliseconds += tp->tv_usec / 1000;
        set_current_millis(total_milliseconds);
    }

    if (tzp != NULL) {
        system_timezone.tz_minuteswest = tzp->tz_minuteswest;
        system_timezone.tz_dsttime = tzp->tz_dsttime;
    }
    return 0;
}
