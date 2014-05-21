The supervisor calls for getting and setting the system time follow
the model of the Unix gettimeofday and settimeofday functions.

- int svc_gettimeofday(struct timeval *tp, void *tzp);
- int svc_settimeofday(const struct timeval *tp, const struct timezone *tzp);

The structures pointed to by tp and tzp are defined in time.h as:

struct timeval {
        time_t       tv_sec;   /* seconds since Jan. 1, 1900 */
        suseconds_t  tv_usec;  /* and microseconds */
};

struct timezone {
        int     tz_minuteswest; /* of Greenwich */
        int     tz_dsttime;     /* type of dst correction to apply */
};

The key difference is that the epoch year is 1900, not 1970.

The tv_usec element of a struct timeval uses microseconds as
its unit for compatibility with the Unix version. However,
SOS only maintains time with millisecond precision.

Note that the setdate shell command only supports Eastern Daylight Time.
Neverthless it is possible to change the timezone and
daylight savings flag via the svc_settimeofday interface.
