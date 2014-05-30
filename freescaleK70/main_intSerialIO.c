#ifdef SERIAL_INTERRUPT_DEMO

#include <stdio.h>
#include "intSerialIO.h"
#include "uart.h"
#include "nvic.h"
#include "../init/init.h"
#include "../include/svc.h"

int main(void) {
    char c;
    initialize_system();

    svc_myFputs("InterruptSerialIO project starting\r\n", STDOUT);

    do {
        svc_myFputs("Enter character: ", STDOUT);
        c = svc_myFgetc(STDIN);
        svc_myFputs("\r\n", STDOUT);
    } while(c != 'x');

    svc_myFputs("InterruptSerialIO project completed\r\n", STDOUT);

    flushBuffer();

    return 0;
}

#endif
