#include "../include/constants.h"
#include "../freescaleK70/io.h"
#include "settimer.h"
#include "../include/svc.h"
#include "../util/util.h"

int interrupt_fired;

void timerAction(void) {
    /* in handler mode, don't use svc call */
    interrupt_fired = 1;
}


#ifdef TEST_SHELL
int cmd_settimer(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_settimer(int argc, char *argv[]) {
#endif
    int res;
    int count;

    if (argc != 2) {
        res = efputs("usage: settimer [count]\r\n", ostrm);
        if (res == EOF) {
            return WRITE_ERROR;
        }
        return WRONG_NUMBER_ARGS;
    }

    count = myAtoi(argv[1]);
    if (count < 0 || count > 65535) {
        res = efputs("Duration must be between 0 and 65535\r\n", ostrm);
        if (res == EOF) {
            return WRITE_ERROR;
        }
        return INVALID_INPUT;
    }

    interrupt_fired = 0;
    svc_setTimer(count);
    while(!interrupt_fired) {
        ;
    }
    efputs("Interrupt Fired\r\n", STDOUT);
    svc_myFflush(ostrm);
    return SUCCESS;
}
