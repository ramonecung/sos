#ifdef PDB_CONTINUOUS_DEMO
/**
 * intPDBContinuousTest.c
 * Programmable Delay Block (PDB) continuous interrupt test program
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
 * that a pointer to the PDB0Isr function is in the vector table at vector 88 (0x0000_0160)
 * for Programmable Data Block (PDB).
 *
 * The following declaration needs to inserted earlier in the file:
 *   extern void PDB0Isr(void);
 *
 * If using the GCC Toolchain, the vector table is named "InterruptVector", and the line:
 *   Default_Handler,		(comment delimiters removed) Vector 88: PDB
 * needs to be changed to:
 *   PDB0Isr,			(comment delimiters removed) Vector 88: PDB
 *
 * If using the Freescale Toolchain, the vector table is named "__vect_table", and the line:
 *   (tIsrFunc)UNASSIGNED_ISR,    (comment delimiters removed) 88 (0x00000160) (prior: -)
 * needs to be changed to:
 *   (tIsrFunc)PDB0Isr,		  (comment delimiters removed) 88 (0x00000160) (prior: -)
 */

#include <stdio.h>
#include "intPDBContinuousTest.h"
#include "delay.h"
#include "led.h"
#include "switchcmd.h"
#include "PDB.h"

/**
 * react to a user command (a button press).  commands are encoded
 * into several constants as defined in switchcmd.h.
 */
void processSwitchCmd(enum switchState cmd) {
	switch(cmd) {
	/* switch further away from the edge of the pc board */
	case switch1Down:
		printf("Stopping PDB0\n");

		PDB0Stop();
		break;
	/* switch closer to the edge of the pc board */
	case switch2Down:
		printf("Starting PDB0\n");

		PDB0Start();
		break;
	case noChange:
	case switch1Up:
	case switch2Up:
		break;        
	}  
}


/**
 * perform overall initialization.  the LCD, button states, scroll counters,
 * and timer are all set to known states and set in motion.
 */
void intPDBContinuousTestInit(void) {
	ledInitAll();
	switchcmdInit();

	/* set interval to 16384 1/16384 second periods or 1 second */
	PDB0Init(16384, PDBTimerContinuous);
}

/**
 * this routine contains all actions to be performed when a PDBTimer 0
 * interrupt occurs.
 *
 * alternate turning the orange LED on and off with each interrupt.
 */
void PDB0Action(void) {
	static char ledState = 0;

	if((ledState ^= 1) == 1)
		ledOrangeOn();
	else
		ledOrangeOff();
}

/**
 * main entry point of program.
 * initialize all hardware.  enter infinite loop that busy-checks
 * for button presses and responds appropriately.
 */
int main(void) {
	enum switchState cmd;

	printf("Starting InterruptPDBContinuousTest project\n");

	/* Perform all the necessary initializations */
	intPDBContinuousTestInit();

	/* Allows interrupts (PRIMASK is cleared) -- this is also the default on reset */
	__asm("cpsie i");
	
	/* Endless main loop */
	while(1) {
		cmd = switchScan();
		if(cmd != noChange) {
			/* Delay for pushbutton debouncing */
			delay(3000);
			processSwitchCmd(cmd);
		}
	}
}
#endif

