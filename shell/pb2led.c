#include "../include/constants.h"
#include "../freescaleK70/io.h"
#include "pb2led.h"
#include "../freescaleK70/hardware/svc.h"
#include "../util/util.h"

#ifdef TEST_SHELL
int cmd_pb2led(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_pb2led(int argc, char *argv[]) {
#endif
    Stream *sw1, *sw2, *sledo, *sledy;
    int res;
    int c, d, e, f;
    int orange_on, yellow_on;
    int status;
    sw1 = svc_myFopen("/dev/button/sw1");
    sw2 = svc_myFopen("/dev/button/sw2");
    sledo = svc_myFopen("/dev/led/orange");
    sledy = svc_myFopen("/dev/led/yellow");

    svc_myFputc(0, sledo);
    orange_on = 0;
    svc_myFputc(0, sledy);
    yellow_on = 0;

    res = efputs("pb2led: ready\r\n", STDOUT);
    if (res != SUCCESS) {
        status = WRITE_ERROR;
        goto cleanup;
    }

    /* Continuously copy from SW1 to orange LED and SW2 to yellow LED. */
    /* End when both SW1 and SW2 are depressed. */
    while(TRUE) {
        c = svc_myFgetc(sw1);
        d = svc_myFgetc(sw2);
        if (c == EOF || d == EOF) {
            res = efputs("pb2led: end of file received from button\r\n", STDOUT);
            status = READ_ERROR;
            goto cleanup;
        }
        if (c && d) {
            res = efputs("pb2led: SW1 and SW2 pressed, exiting\r\n", STDOUT);
            status = SUCCESS;
            goto cleanup;
        }
        if (c) {
            if (orange_on) {
                orange_on = 0;
                e = svc_myFputc(0, sledo);
            } else {
                orange_on = 1;
                e = svc_myFputc(1, sledo);
            }
            if (e == EOF) {
               status = WRITE_ERROR;
               goto cleanup;
            }
        }
        if (d) {
            if (yellow_on) {
                yellow_on = 0;
                f = svc_myFputc(0, sledy);
            } else {
                yellow_on = 1;
                f = svc_myFputc(1, sledy);
            }
            if (f == EOF) {
                status = WRITE_ERROR;
                goto cleanup;
            }
        }
    }

    cleanup:
        svc_myFputc(0, sledo);
        svc_myFputc(0, sledy);
        svc_myFclose(sw1);
        svc_myFclose(sw2);
        svc_myFclose(sledo);
        svc_myFclose(sledy);
        return status;
}
