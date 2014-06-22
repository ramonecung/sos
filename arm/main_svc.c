#ifdef SVC_DEMO
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
 *   SVC_Handler,            (comment delimiters removed) Vector 11: SuperVisor Call (SVCall)
 * needs to be changed to:
 *   svcHandler,            (comment delimiters removed) Vector 11: SuperVisor Call (SVCall)
 *
 * If using the Freescale Toolchain, the vector table is named "__vect_table", and the line:
 *   (tIsrFunc)UNASSIGNED_ISR,   (comment delimiters removed) 11 (0x0000002C) (prior: -)
 * needs to be changed to:
 *   (tIsrFunc)svcHandler,       (comment delimiters removed) 11 (0x0000002C) (prior: -)
 */

#include "../include/svc.h"
#include "../init/init.h"
#include "../util/util.h"
#include "../include/io.h"

int main(void) {
    int j;
    void *vp;
    Stream *s1, *s2, *s3;

    initialize_system();

    efputs("\r\nStarting SVCall project\r\n", STDOUT);


    vp = svc_myMalloc(8);
    svc_myFree(vp);


    efputs("Opening Orange LED and writing to it\r\n", STDOUT);
    s1 = svc_myFopen("/dev/led/orange");
    svc_myFputc(1, s1);
    svc_myFclose(s1);

    efputs("Opening SW1 button and reading from it\r\n", STDOUT);
    s2 = svc_myFopen("/dev/button/sw1");
    j = svc_myFgetc(s2);
    if (j) {
        efputs("Got 1 from SW1\r\n", STDOUT);
    } else {
        efputs("Got 0 from SW1\r\n", STDOUT);
    }
    svc_myFclose(s2);

    efputs("Creating in-memory file /dev/fs/data and opening it\r\n", STDOUT);
    svc_myCreate("/dev/fs/data");
    s3 = svc_myFopen("/dev/fs/data");
    efputs("Writing characters 'a' and 'b' to /dev/fs/data\r\n", STDOUT);
    svc_myFputc('a', s3);
    svc_myFputc('b', s3);
    efputs("Reading characters from /dev/fs/data\r\n", STDOUT);
    j = svc_myFgetc(s3);
    efputs("Got: ", STDOUT);
    efputc(j, STDOUT);
    efputc('\n', STDOUT);
    j = svc_myFgetc(s3);
    efputs("Got: ", STDOUT);
    efputc(j, STDOUT);
    efputc('\n', STDOUT);
    efputs("Closing and deleting /dev/fs/data\r\n", STDOUT);
    svc_myFclose(s3);
    svc_myDelete("/dev/fs/data");

    efputs("Exiting SVCall project\r\n", STDOUT);
    svc_myFflush(STDOUT);

    return 0;
}

#endif

