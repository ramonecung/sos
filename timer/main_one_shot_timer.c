#ifdef PDB_ONESHOT_DEMO

#include <stdio.h>
#include "one_shot_timer.h"
#include "../freescaleK70/hardware/delay.h"
#include "../freescaleK70/hardware/led.h"
#include "../freescaleK70/hardware/PDB.h"
#include "../freescaleK70/io.h"
#include "../init/init.h"
#include "../freescaleK70/hardware/svc.h"


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

	/* Perform all the necessary initializations */
	svc_setTimer(46875);
	/* Allows interrupts (PRIMASK is cleared) -- this is also the default on reset */
	__asm("cpsie i");
	
	/* delay long enough to let the timer go off */
	delay(15000000);
	svc_myFputs("Ending InterruptPDBOneShotTest project\r\n", STDOUT);
	svc_flushOutput();
	return 0;
}
#endif
