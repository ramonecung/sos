#ifdef ONESHOT_TIMER_DEMO

#include <stdint.h>
#include "one_shot_timer.h"
#include "../freescaleK70/delay.h"
#include "../freescaleK70/led.h"
#include "../freescaleK70/PDB.h"
#include "../include/io.h"
#include "../init/init.h"
#include "../include/svc.h"


/**
 * this routine contains all actions to be performed when the timer expires
 *
 */
void timerAction(void) {
	/* in handler mode, don't use svc call */
	myFputs("Interrupt Fired\r\n", STDOUT);
}


/**
 * main entry point of program.
 * initialize all hardware.  enter infinite loop that busy-checks
 * for button presses and responds appropriately.
 */
int main(void) {
	initialize_system();
	svc_myFputs("Starting InterruptPDBOneShotTest project\r\n", STDOUT);

	/* 46875 is 1 second */
	svc_myFputs("Scheduling interrupt in 46875/46875 seconds\r\n", STDOUT);
	svc_setTimer(46875);
	/* delay long enough to let the timer go off */
	delay(30000000);

	svc_myFputs("Scheduling interrupt in 4687/46875 seconds\r\n", STDOUT);
	svc_setTimer(4687);
	/* delay long enough to let the timer go off */
	delay(15000000);

	svc_myFputs("Ending InterruptPDBOneShotTest project\r\n", STDOUT);
	svc_flushOutput();
	return 0;
}
#endif
