#ifdef FLEX_TIMER_DEMO

#include <stdio.h>
#include "hardware/delay.h"
#include "hardware/svc.h"
#include "../init/init.h"
#include "time.h"
#include "io.h"

int main(void) {
    char buf[128];
    struct timeval tv;
    struct timezone tz;
    int result;

    initialize_system();
    svc_myFputs("Starting InterruptFlexTimer based time of day test\r\n", STDOUT);

    tv.tv_sec = 1400427669;
    tv.tv_usec = 0;
    tz.tz_minuteswest = 300;
    tz.tz_dsttime = 1;

    svc_myFputs("Calling settimeofday\r\n", STDOUT);
    result = svc_settimeofday(&tv, &tz);
    sprintf(buf, "result: %d, "
            "tv.tv_sec: %lld, tv.tv_usec: %lld, "
            "tz.tz_minutwest: %d, tz.tz_dsttime: %d\r\n",
            result, tv.tv_sec, tv.tv_sec, tz.tz_minuteswest, tz.tz_dsttime);
    svc_myFputs(buf, STDOUT);

    svc_myFputs("Starting gettimeofday loop\r\n", STDOUT);
    /* Endless main loop */
    while(1) {
        result = svc_gettimeofday(&tv, &tz);
        sprintf(buf, "result: %d, "
                "tv.tv_sec: %lld, tv.tv_usec: %lld, "
                "tz.tz_minutwest: %d, tz.tz_dsttime: %d\r\n",
                result, tv.tv_sec, tv.tv_usec, tz.tz_minuteswest, tz.tz_dsttime);
        svc_myFputs(buf, STDOUT);
        asmDelay(30000000);
    }
}
#endif
