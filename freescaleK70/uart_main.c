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


#include "io.h"
#include "../init/init.h"
#include "../util/util.h"

int main(void) {
    char c;
    
    efputs("SerialIO Project Starting\n", stdout);
    initialize_system();

    Stream *s;
    s = myFopen("/dev/uart/uart2");
    c = myFgetc(s);

    myFputc('c', s);
    myFputc(':', s);
    myFputc(c, s);
    myFputc('\r', s);
    myFputc('\n', s);

    efputs("SerialIO Project Completed\n", stdout);

    return 0;
}
#endif
