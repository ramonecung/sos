#include "../include/constants.h"
#include "../include/io.h"
#include "therm2ser.h"
#include "../include/svc.h"
#include "../freescaleK70/delay.h"
#include "../util/util.h"

#ifdef TEST_SHELL
int cmd_therm2ser(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_therm2ser(int argc, char *argv[]) {
#endif
    Stream *sw1, *stherm, *suart;
    int res;
    int c;
    char formatted_output[64];
    int status;
    sw1 = svc_myFopen("/dev/button/sw1");
    stherm = svc_myFopen("/dev/adc/thermistor");
    suart = svc_myFopen("/dev/uart/uart2");
    /* Continuously output the value of the thermistor */
    /* to the serial device as a decimal number followed by a newline. */
    /* End when SW1 is depressed. */
    while(!svc_myFgetc(sw1)) {
        c = svc_myFgetc(stherm);
        if (c == EOF) {
            res = svc_myFputs("therm2ser: end of file\r\n", suart);
            status = READ_ERROR;
            goto cleanup;
        }
        sprintf(formatted_output, "%4u\r\n", c);
        res = svc_myFputs(formatted_output, suart);
        if (res != SUCCESS) {
            status = WRITE_ERROR;
            goto cleanup;
        }
        delay(5000000);
    }

    res = svc_myFputs("therm2ser: SW1 pressed, exiting\r\n", suart);
    status = SUCCESS;
    goto cleanup;

    cleanup:
        svc_myFclose(sw1);
        svc_myFclose(stherm);
        svc_myFclose(suart);
        return status;
}
