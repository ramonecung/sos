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
 * Written by James L. Frankel (frankel@seas.harvard.edu)
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

#include <stdio.h>
#include "hardware/svc.h"
#include "../memory/memory.h"
#include "../init/init.h"
#include "io.h"

/* Issue the SVC (Supervisor Call) instruction (See A7.7.175 on page A7-503 of the
 * ARM®v7-M Architecture Reference Manual, ARM DDI 0403Derrata 2010_Q3 (ID100710)) */
#ifdef __GNUC__
void __attribute__((naked)) pushFourBytesThenSVCEndive(void) {
	__asm("\n\
#	Force non-eight-byte alignment\n\
	push	{r6}\n\
	svc		%0\n\
#	Pop off the pushed non-eight-byte alignment word\n\
	pop		{r6}\n\
	bx		lr\n\
		" : : "I" (SVC_ENDIVE));
}
#else
__asm void pushFourBytesThenSVCEndive(void) {
//	Force non-eight-byte alignment
	push	r6
	svc		#SVC_ENDIVE
//	Pop off the pushed non-eight-byte alignment word
	pop		r6
	bx		lr
}
#endif

int main(void) {
	int i, j;
	void *vp;
	Stream *s1, *s2, *s3;
	
	printf("Starting SVCall project\n");
	initialize_system();
	initialize_memory();
	initialize_io();
	/* Set the SVC handler priority */
	svcInit_SetSVCPriority(7);
	
	vp = svc_myMalloc(8);
	svc_myFree(vp);

	
	s1 = svc_myFopen("/dev/led/orange");
	i = svc_myFputc(1, s1);
	svc_myFclose(s1);
	
	s2 = svc_myFopen("/dev/button/sw1");
	j = svc_myFgetc(s2);
	printf("Got %d from SW1\n", j);
	svc_myFclose(s2);

	svc_myCreate("/dev/fs/data");
	s3 = svc_myFopen("/dev/fs/data");
	i = svc_myFputc('a', s3);
	i = svc_myFputc('b', s3);
	j = svc_myFgetc(s3);
	printf("Got %c from in memory file\n", j);
	j = svc_myFgetc(s3);
	printf("Got %c from in memory file\n", j);
	svc_myFclose(s3);
	svc_myDelete("/dev/fs/data");
	
	printf("Exiting SVCall project\n");
	
	return 0;
}
