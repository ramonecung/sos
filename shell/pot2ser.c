#include "../include/constants.h"
#include "../include/io.h"
#include "pot2ser.h"
#include "../include/svc.h"
#include "../freescaleK70/hardware/delay.h"
#include "../util/util.h"

#define POTENTIOMETER_ZERO 48 /* values bottom out at 45 - 48 */

#ifdef TEST_SHELL
int cmd_pot2ser(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_pot2ser(int argc, char *argv[]) {
#endif
    Stream *s1, *s2;
    int res;
    int c;
    char formatted_output[64];
    s1 = svc_myFopen("/dev/adc/potentiometer");
    s2 = svc_myFopen("/dev/uart/uart2");
    /* Continuously output the value of the analog potentiomemter */
    /* to the serial device as a decimal number followed by a newline. */
    while(TRUE) {
        c = svc_myFgetc(s1);
        if (c == EOF) {
            res = svc_myFputs("pot2ser: end of file\r\n", s2);
            return READ_ERROR;
        }
        sprintf(formatted_output, "%4u\r\n", c);
        res = svc_myFputs(formatted_output, s2);
        if (res != SUCCESS) {
            return WRITE_ERROR;
        }
        /* End on a value of zero */
        if (c <= POTENTIOMETER_ZERO) {
            res = svc_myFputs("pot2ser: reached min potentiometer value, exiting\r\n", s2);
            break;
        }
        delay(10000000);
    }

    svc_myFclose(s1);
    svc_myFclose(s2);
    return SUCCESS;
}
