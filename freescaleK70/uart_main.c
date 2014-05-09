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

#include "derivative.h"
#include "hardware/uart.h"
#include "hardware/delay.h"
#include "../init/init.h"
#include "../util/util.h"

int main(void) {
    const unsigned long int delayCount = 0x7ffff;

    /* On reset (i.e., before calling mcgInit), the processor clocking
     * starts in FEI (FLL Engaged Internal) mode.  In FEI mode and with
     * default settings (DRST_DRS = 00, DMX32 = 0), the MCGFLLCLK, the
     * MCGOUTCLK (MCG (Multipurpose Clock Generator) clock), and the Bus
     * (peripheral) clock are all set to 640 * IRC.  IRC is the Internal
     * Reference Clock which runs at 32 KHz. [See K70 Sub-Family
     * Reference Manual, Rev. 2, Section 25.4.1.1, Table 25-22 on
     * page 657 and MCG Control 4 Register (MCG_C4) Section 25.3.4 on
     * page 641] */

    /* After calling mcgInit, MCGOUTCLK is set to 120 MHz and the Bus
     * (peripheral) clock is set to 60 MHz.*/

    /* Table 5-2 on page 221 indicates that the clock used by UART0 and
     * UART1 is the System clock (i.e., MCGOUTCLK) and that the clock
     * used by UART2-5 is the Bus clock. */
    const int IRC = 32000;                  /* Internal Reference Clock */
    const int FLL_Factor = 640;
    const int moduleClock = FLL_Factor*IRC;
    const int busClock = 60000000;
    const int KHzInHz = 1000;

    const int IRCBaud = 9600;
    const int busBaud = 115200;

    char c;
    
    efputs("SerialIO Project Starting\n", stdout);
    initialize_system();
    if (system_initialized()) {
    	uartInit(UART2_BASE_PTR, busClock/KHzInHz, busBaud);
    } else {
    	uartInit(UART2_BASE_PTR, moduleClock/KHzInHz, IRCBaud);
    }
    uartPuts(UART2_BASE_PTR, "SerialIO Project Starting\r\n");

    uartPuts(UART2_BASE_PTR, "Waiting for character from UART2");

    while(!uartGetcharPresent(UART2_BASE_PTR)) {
        uartPutchar(UART2_BASE_PTR, '.');
        delay(delayCount);
    }

    c = uartGetchar(UART2_BASE_PTR);
    uartPuts(UART2_BASE_PTR, "\r\nReceived character from UART2: '");
    uartPutchar(UART2_BASE_PTR, c);
    uartPuts(UART2_BASE_PTR, "'\r\n");

    uartPuts(UART2_BASE_PTR, "SerialIO Project Completed\r\n");

   efputs("SerialIO Project Completed\n", stdout);

    return 0;
}
#endif
