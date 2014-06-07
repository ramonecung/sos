#ifdef UART_DEMO

/**
 * main.c
 * UART2 Serial I/O demonstration project main program
 *
 * Demonstrates using UART2 serial I/O
 *
 * ARM-based K70F120M microcontroller board
 *   for educational purposes only
 * CSCI E-92 Spring 2014, Professor James L. Frankel, Harvard Extension School
 *
 * Written by James L. Frankel (frankel@seas.harvard.edu)
 */


#include "../include/io.h"
#include "../init/init.h"
#include "../util/util.h"
#include "../include/svc.h"

int main(void) {
    initialize_system();

    efputs("SerialIO Project Starting\r\n", STDOUT);
    Stream *s;
    s = svc_myFopen("/dev/uart/uart2");

    svc_myFputs("enter character: ", s);
    svc_myFgetc(s);
    svc_myFputs("\r\n", s);

    efputs("SerialIO Project Completed\r\n", STDOUT);
    svc_myFflush(STDOUT);
    return 0;
}
#endif
