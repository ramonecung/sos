#include "../include/constants.h"
#include "../freescaleK70/io.h"
#include "touch2led.h"
#include "../include/svc.h"

#ifdef TEST_SHELL
int cmd_touch2led(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_touch2led(int argc, char *argv[]) {
#endif
    Stream *s1, *s2, *s3, *s4, *sledo, *sledy, *sledg, *sledb;
    int number_depressed;
    s1 = svc_myFopen("/dev/touch/e1");
    s2 = svc_myFopen("/dev/touch/e2");
    s3 = svc_myFopen("/dev/touch/e3");
    s4 = svc_myFopen("/dev/touch/e4");
    sledo = svc_myFopen("/dev/led/orange");
    sledy = svc_myFopen("/dev/led/yellow");
    sledg = svc_myFopen("/dev/led/green");
    sledb = svc_myFopen("/dev/led/blue");

    /* Continuously copy from each touch sensor to the corresponding LED. */
    /* End when all four touch sensors are "depressed." */

    while(1) {
        number_depressed = 0;
        if(svc_myFgetc(s1)) {
            svc_myFputc(1, sledo);
            number_depressed++;
        } else {
            svc_myFputc(0, sledo);
        }
        if(svc_myFgetc(s2)) {
            svc_myFputc(1, sledy);
            number_depressed++;
        } else {
            svc_myFputc(0, sledy);
        }
        if(svc_myFgetc(s3)) {
            svc_myFputc(1, sledg);
            number_depressed++;
        } else {
            svc_myFputc(0, sledg);
        }
        if(svc_myFgetc(s4)) {
            svc_myFputc(1, sledb);
            number_depressed++;
        } else {
            svc_myFputc(0, sledb);
        }
        if (number_depressed >= 4) {
            svc_myFputc(0, sledo);
            svc_myFputc(0, sledy);
            svc_myFputc(0, sledg);
            svc_myFputc(0, sledb);

            svc_myFclose(s1);
            svc_myFclose(s2);
            svc_myFclose(s3);
            svc_myFclose(s4);

            svc_myFclose(sledo);
            svc_myFclose(sledy);
            svc_myFclose(sledg);
            svc_myFclose(sledb);

            return SUCCESS;
        }
    }
}
