#ifdef FLEX_TIMER_DEMO
/**
 * intFlexTimerTest.c
 * FlexTimer interrupt test program
 *
 * ARM-based K70F120M microcontroller board
 *   for educational purposes only
 * CSCI E-92 Spring 2014, Professor James L. Frankel, Harvard Extension School
 *
 * Written by James L. Frankel (frankel@seas.harvard.edu)
 */

/*
 * Important note:
 *
 * The file Project_Settings -> Startup_Code -> kinetis_sysinit.c needs to be modified so
 * that a pointer to the flexTimer0Isr function is in the vector table at vector 78 (0x0000_0138)
 * for Flex Timer 0 (FTM0).
 *
 * The following declaration needs to inserted earlier in the file:
 *   extern void flexTimer0Isr(void);
 *
 * If using the GCC Toolchain, the vector table is named "InterruptVector", and the line:
 *   Default_Handler,		(comment delimiters removed) Vector 78: FTM0
 * needs to be changed to:
 *   flexTimer0Isr,		(comment delimiters removed) Vector 78: FTM0
 *
 * If using the Freescale Toolchain, the vector table is named "__vect_table", and the line:
 *   (tIsrFunc)UNASSIGNED_ISR,    (comment delimiters removed) 78 (0x00000138) (prior: -)
 * needs to be changed to:
 *   (tIsrFunc)flexTimer0Isr,	  (comment delimiters removed) 78 (0x00000138) (prior: -)
 */

#include <stdio.h>
#include "hardware/delay.h"
#include "hardware/svc.h"
#include "../init/init.h"
#include "time.h"
#include "io.h"


/**
 * main entry point of program.
 * initialize all hardware.  enter infinite loop that busy-checks
 * for button presses and responds appropriately.
 */
int main(void) {
    initialize_system();
	svc_myFputs("Starting InterruptFlexTimerTest project\r\n", STDOUT);
	/* Endless main loop */
	char buf[64];
	unsigned long long tv;
	while(1) {
		tv = current_millis();
		sprintf(buf, "%llu\r\n", tv);
		svc_myFputs(buf, STDOUT);
		asmDelay(30000000);
	}

/*
    struct timeval *tv;
    struct timezone *tz;

    tv = emalloc(sizeof(struct timeval), "gettimeofday", STDERR);
    tz = emalloc(sizeof(struct timezone), "gettimeofday", STDERR);
    if (tv == NULL || tz == NULL) {
        return -1;
    }
*/
}
#endif
