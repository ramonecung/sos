#include <stdint.h>
#include "time.h"
#include "../include/constants.h"
#include "../util/util.h"

#define YEAR_1900_SECONDS 0
#define YEAR_3000_SECONDS 32503680000
#define ONE_MILLION_USECONDS 1000000
/*
 * Need to support years 1900 to 3000
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
uint64_t milliseconds_since_epoch;

/* TODO: stop hardcoding this to EDT */
struct timezone system_timezone = { 300, 1 };

/* set flextimer to interrupt every millisecond and increment */

/*
 * this routine contains all actions to be performed when a FlexTimer 0
 * interrupt occurs.
 */
void flexTimer0Action(void) {
    /* di(); */
    milliseconds_since_epoch++;
    /* ei(); */
}

uint64_t get_current_millis(void) {
    /* di(); */
    return milliseconds_since_epoch;
    /* ei(); */
}

void set_current_millis(uint64_t millis) {
    /* di() */
    milliseconds_since_epoch = millis;
    /* ei() */
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
        tp->sec = total_milliseconds / 1000;
        remaining_milliseconds = total_milliseconds - (1000 * tp->sec);
        tp->usec = remaining_milliseconds * 1000;
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
 * A 0 return value indicates that the call succeeded.
 * A -1 return value indicates an error occurred
 */
int settimeofday(const struct timeval *tp, const struct timezone *tzp) {
    uint64_t total_milliseconds;
    if (tp != NULL) {
        /* basic error checking */
        if (tp->sec < YEAR_1900_SECONDS || tp->sec > YEAR_3000_SECONDS) {
            return -1;
        }
        if (tp->usec < 0 || tp->usec >= ONE_MILLION_USECONDS) {
            return -1;
        }
        total_milliseconds = tp->sec * 1000;
        total_milliseconds += tp->usec / 1000;
        set_current_millis(total_milliseconds);
    }

    if (tzp != NULL) {
        system_timezone.tz_minuteswest = tzp->tz_minuteswest;
        system_timezone.tz_dsttime = tzp->tz_dsttime;
    }
    return 0;
}
