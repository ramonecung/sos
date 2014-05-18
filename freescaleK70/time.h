#ifndef TIME_H
#define TIME_H

#include <stdint.h>

typedef signed long long int time_t;
typedef signed long long int suseconds_t;

struct timeval {
    time_t tv_sec;       /* seconds since Jan. 1, 1900 */
    suseconds_t tv_usec; /* and microseconds */
    /* NOTE: SOS does not maintain microsecond precision */
};

struct timezone {
    int tz_minuteswest; /* of Greenwich */
    int tz_dsttime;     /* type of dst correction to apply */
};

void intFlexTimerInit(void);

void set_current_millis(uint64_t millis);
uint64_t get_current_millis(void);

int gettimeofday(struct timeval *tp, void *tzp);
int settimeofday(const struct timeval *tp, const struct timezone *tzp);

#endif
