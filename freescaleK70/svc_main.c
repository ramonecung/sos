/**
 * main.c
 * Supervisor call demonstration project main program
 *
 * Demonstrates using the K70 supervisor call instruction
 *
 * ARM-based K70F120M microcontroller board
 *   for educational purposes only
 * CSCI E-92 Spring 2014, Professor James L. Frankel, Harvard Extension School
 *
 * Written by James L. Frankel (frankel@seas.harvard.edu) and Stephen Karger
 */

/*
 * Important note:
 *
 * The file Project_Settings -> Startup_Code -> kinetis_sysinit.c needs to be modified so
 * that a pointer to the svcHandler function is in the vector table at vector 11 (0x0000002C)
 * for the SuperVisor Call (SVCall).
 *
 * The following declaration needs to inserted earlier in the file:
 *   extern void svcHandler(void);
 *
 * If using the GCC Toolchain, the vector table is named "InterruptVector", and the line:
 *   SVC_Handler,			(comment delimiters removed) Vector 11: SuperVisor Call (SVCall)
 * needs to be changed to:
 *   svcHandler,			(comment delimiters removed) Vector 11: SuperVisor Call (SVCall)
 *
 * If using the Freescale Toolchain, the vector table is named "__vect_table", and the line:
 *   (tIsrFunc)UNASSIGNED_ISR,   (comment delimiters removed) 11 (0x0000002C) (prior: -)
 * needs to be changed to:
 *   (tIsrFunc)svcHandler,       (comment delimiters removed) 11 (0x0000002C) (prior: -)
 */

#ifdef SVC_DEMO

#include "hardware/svc.h"
#include "../memory/memory.h"
#include "../init/init.h"
#include "../util/util.h"
#include "io.h"

int main(void) {
	int j;
	void *vp;
	Stream *s1, *s2, *s3;

	efputs("Starting SVCall project\n", stdout);
	initialize_system();
	initialize_memory();
	initialize_io();
	/* Set the SVC handler priority */
	svcInit_SetSVCPriority(7);

	vp = svc_myMalloc(8);
	svc_myFree(vp);


	efputs("Opening Orange LED and writing to it\n", stdout);
	s1 = svc_myFopen("/dev/led/orange");
	svc_myFputc(1, s1);
	svc_myFclose(s1);

	efputs("Opening SW1 button and reading from it\n", stdout);
	s2 = svc_myFopen("/dev/button/sw1");
	j = svc_myFgetc(s2);
	if (j) {
		efputs("Got 1 from SW1\n", stdout);
	} else {
		efputs("Got 0 from SW1\n", stdout);
	}
	svc_myFclose(s2);

	efputs("Creating in-memory file /dev/fs/data and opening it\n", stdout);
	svc_myCreate("/dev/fs/data");
	s3 = svc_myFopen("/dev/fs/data");
	efputs("Writing characters 'a' and 'b' to /dev/fs/data\n", stdout);
	svc_myFputc('a', s3);
	svc_myFputc('b', s3);
	efputs("Reading characters from /dev/fs/data\n", stdout);
	j = svc_myFgetc(s3);
	efputs("Got: ", stdout);
	efputc(j, stdout);
	efputc('\n', stdout);
	j = svc_myFgetc(s3);
	efputs("Got: ", stdout);
	efputc(j, stdout);
	efputc('\n', stdout);
	efputs("Closing and deleting /dev/fs/data\n", stdout);
	svc_myFclose(s3);
	svc_myDelete("/dev/fs/data");

	efputs("Exiting SVCall project\n", stdout);

	return 0;
}

#endif
