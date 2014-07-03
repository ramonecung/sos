/**
 * svc.c
 * Routines for supervisor calls
 *
 * ARM-based K70F120M microcontroller board
 *   for educational purposes only
 * CSCI E-92 Spring 2014, Professor James L. Frankel, Harvard Extension School
 *
 * Written by James L. Frankel (frankel@seas.harvard.edu) and Stephen Karger
 */

/*
 * Size of the supervisor call stack frame (no FP extension):
 *   No alignment => 32 (0x20) bytes
 *   With alignment => 36 (0x24) bytes
 *
 * Format of the supervisor call stack frame (no FP extension):
 *   SP Offset   Contents
 *   +0             R0
 *   +4             R1
 *   +8             R2
 *   +12         R3
 *   +16         R12
 *   +20         LR (R14)
 *   +24         Return Address
 *   +28         xPSR (bit 9 indicates the presence of a reserved alignment
 *                      word at offset +32)
 *   +32         Possible Reserved Word for Alignment on 8 Byte Boundary
 *
 * Size of the supervisor call stack frame (with FP extension):
 *   No alignment => 104 (0x68) bytes
 *   With alignment => 108 (0x6C) bytes
 *
 * Format of the supervisor call stack frame (with FP extension):
 *   SP Offset   Contents
 *   +0             R0
 *   +4             R1
 *   +8             R2
 *   +12         R3
 *   +16         R12
 *   +20         LR (R14)
 *   +24         Return Address
 *   +28         xPSR (bit 9 indicates the presence of a reserved alignment
 *                      word at offset +104)
 *   +32         S0
 *   +36         S1
 *   +40         S2
 *   +44         S3
 *   +48         S4
 *   +52         S5
 *   +56         S6
 *   +60         S7
 *   +64         S8
 *   +68         S9
 *   +72         S10
 *   +76         S11
 *   +80         S12
 *   +84         S13
 *   +88         S14
 *   +92         S15
 *   +96         FPSCR
 *   +100         Reserved Word for 8 Byte Boundary of Extended Frame
 *   +104         Possible Reserved Word for Alignment on 8 Byte Boundary
 */

#include "derivative.h"
#include <stdio.h>
#include "../memory/memory.h"
#include "../time/time.h"
#include "../timer/one_shot_timer.h"
#include "../include/io.h"
#include "../process/process.h"

#include "critical_section.h"
#include "svc_internal.h"

#define XPSR_FRAME_ALIGNED_BIT 9
#define XPSR_FRAME_ALIGNED_MASK (1<<XPSR_FRAME_ALIGNED_BIT)

 // Implemented SVC numbers
#define SVC_FREE 0
#define SVC_MALLOC 1
#define SVC_FPUTC 2
#define SVC_FPUTS 3
#define SVC_FGETC 4
#define SVC_FGETS 5
#define SVC_FOPEN 6
#define SVC_FCLOSE 7
#define SVC_CREATE 8
#define SVC_DELETE 9
#define SVC_GET_CURRENT_MILLIS 10
#define SVC_GETTIMEOFDAY 11
#define SVC_SETTIMEOFDAY 12
#define SVC_SETTIMER 13
#define SVC_FLUSHOUTPUT 14
#define SVC_SPAWN 15
#define SVC_YIELD 16
#define SVC_BLOCK 17
#define SVC_WAKE 18
#define SVC_KILL 19

struct frame {
    union {
        int r0;
        int arg0;
        int returnVal;
    };
    union {
        int r1;
        int arg1;
    };
    union {
        int r2;
        int arg2;
    };
    union {
        int r3;
        int arg3;
    };
    int r12;
    union {
        int r14;
        int lr;
    };
    int returnAddr;
    int xPSR;
};

/* Issue the SVC (Supervisor Call) instruction (See A7.7.175 on page A7-503 of the
 * ARM®v7-M Architecture Reference Manual, ARM DDI 0403Derrata 2010_Q3 (ID100710)) */
#ifdef __GNUC__
void __attribute__((naked)) __attribute__((noinline)) internal_svc_myFree(void *ptr) {
    __asm("svc %0" : : "I" (SVC_FREE));
    __asm("bx lr");
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
void * __attribute__((naked)) __attribute__((noinline)) internal_svc_myMalloc(unsigned int size) {
    __asm("svc %0" : : "I" (SVC_MALLOC));
    __asm("bx lr");
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
int __attribute__((naked)) __attribute__((noinline)) internal_svc_myFputc(int c, Stream *stream) {
    __asm("svc %0" : : "I" (SVC_FPUTC));
    __asm("bx lr");
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
int __attribute__((naked)) __attribute__((noinline)) internal_svc_myFputs(const char *s, Stream *stream) {
    __asm("svc %0" : : "I" (SVC_FPUTS));
    __asm("bx lr");
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
int __attribute__((naked)) __attribute__((noinline)) internal_svc_myFgetc(Stream *stream) {
    __asm("svc %0" : : "I" (SVC_FGETC));
    __asm("bx lr");
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
char * __attribute__((naked)) __attribute__((noinline)) internal_svc_myFgets(char *str, int size, Stream *stream) {
    __asm("svc %0" : : "I" (SVC_FGETS));
    __asm("bx lr");
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
Stream * __attribute__((naked)) __attribute__((noinline)) internal_svc_myFopen(const char *filename) {
    __asm("svc %0" : : "I" (SVC_FOPEN));
    __asm("bx lr");
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
int __attribute__((naked)) __attribute__((noinline)) internal_svc_myFclose(Stream *stream) {
    __asm("svc %0" : : "I" (SVC_FCLOSE));
    __asm("bx lr");
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
int __attribute__((naked)) __attribute__((noinline)) internal_svc_myCreate(const char *filename) {
    __asm("svc %0" : : "I" (SVC_CREATE));
    __asm("bx lr");
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
int __attribute__((naked)) __attribute__((noinline)) internal_svc_myDelete(const char *filename) {
    __asm("svc %0" : : "I" (SVC_DELETE));
    __asm("bx lr");
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
uint64_t __attribute__((naked)) __attribute__((noinline)) internal_svc_get_current_millis(void) {
    __asm("svc %0" : : "I" (SVC_GET_CURRENT_MILLIS));
    __asm("bx lr");
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
int __attribute__((naked)) __attribute__((noinline)) internal_svc_gettimeofday(struct timeval *tp, void *tzp) {
    __asm("svc %0" : : "I" (SVC_GETTIMEOFDAY));
    __asm("bx lr");
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
int __attribute__((naked)) __attribute__((noinline)) internal_svc_settimeofday(const struct timeval *tp, const struct timezone *tzp) {
    __asm("svc %0" : : "I" (SVC_SETTIMEOFDAY));
    __asm("bx lr");
}
#pragma GCC diagnostic pop

void __attribute__((naked)) __attribute__((noinline)) internal_svc_setTimer(uint32_t count, void (*function)(void)) {
    __asm("svc %0" : : "I" (SVC_SETTIMER));
    __asm("bx lr");
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
int __attribute__((naked)) __attribute__((noinline)) internal_svc_myFflush(Stream *stream) {
    __asm("svc %0" : : "I" (SVC_FLUSHOUTPUT));
    __asm("bx lr");
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
int __attribute__((naked)) __attribute__((noinline)) internal_svc_spawn(int (*mainfunc)(int argc, char **argv)) {
    __asm("svc %0" : : "I" (SVC_SPAWN));
    __asm("bx lr");
}
#pragma GCC diagnostic pop

void __attribute__((naked)) __attribute__((noinline)) internal_svc_yield(void) {
    __asm("svc %0" : : "I" (SVC_YIELD));
    __asm("bx lr");
}

void __attribute__((naked)) __attribute__((noinline)) internal_svc_block(void) {
    __asm("svc %0" : : "I" (SVC_BLOCK));
    __asm("bx lr");
}

void __attribute__((naked)) __attribute__((noinline)) internal_svc_wake(uint32_t pid) {
    __asm("svc %0" : : "I" (SVC_WAKE));
    __asm("bx lr");
}

void __attribute__((naked)) __attribute__((noinline)) internal_svc_myKill(uint32_t pid) {
    __asm("svc %0" : : "I" (SVC_KILL));
    __asm("bx lr");
}

#endif



/* Exception return behavior is detailed in B1.5.8 on page B1-652 of the
 * ARM®v7-M Architecture Reference Manual, ARM DDI 0403Derrata 2010_Q3
 * (ID100710) */

/* When an SVC instruction is executed, the following steps take place:
 * (1) A stack frame is pushed on the current stack (either the main or
 *     the process stack, depending on how the system is configured)
 *     containing the current values of R0-R3, R12, LR, the return
 *     address for the SVC instruction, xPSR, and, if appropriate, the
 *     floating point registers,
 * (2) An appropriate value is loaded into the LR register indicating
 *     that a stack frame is present on the stack (this will cause a
 *     special return sequence later when this value is loaded into
 *     the PC),
 * (3) Values of R0-R3 and R12 are no longer valid,
 * (4) The PC is set to the address in the interrupt vector table for
 *        the interrupt service routine for the SVC instruction,
 * (5) The processor switches to Handler mode (code execution in
 *     Handler mode is always privileged),
 * (6) The xPSR is set to indicate appropriate SVC state,
 * (7) The processor switches to the main stack by now using the main
 *        stack pointer.
 *
 * These steps are discussed in detail in the pseudo-code given for
 * processor action ExceptionEntry() on page B1-643 of the ARM®v7-M
 * Architecture Reference Manual, ARM DDI 0403Derrata 2010_Q3
 * (ID100710).  ExceptionEntry() invokes PushStack() and
 * ExceptionTaken() on page B1-643. */

/* The following assembler function makes R0 point to the top-of-stack
 * for the stack that was current (either the main or the process stack)
 * before the SVC interrupt service routine began running.  This is
 * where the stack frame was stored by the SVC instruction.  Then, this
 * function calls the svcHandlerInC function.  Note that when a C
 * function is called, R0 contains the first parameter.  Therefore, the
 * first parameter passed to svcHandlerInC is a pointer to the
 * top-of-stack of the stack containing the stack frame. */
void svcHandlerInC(struct frame *framePtr);

#ifdef __GNUC__
void __attribute__((naked)) svcHandler(void) {
    __asm("\n\
            tst        lr, #4\n\
            ite        eq\n\
            mrseq    r0, msp\n\
            mrsne    r0, psp\n\
            push    {lr}\n\
            bl        svcHandlerInC\n\
            pop        {pc}\n\
            ");
}
#endif

void svcHandlerInC(struct frame *framePtr) {
    /* framePtr will be in R0 */
    /* if that changes because of interrupts it causes a race condition */
    /* it is ok for control to switch away from this function to a different */
    /* process, but the other process cannot enter this function until the */
    /* original call finishes. */
    /* this is enforced by the external SVC interface */

    /* framePtr->returnAddr is the return address for the SVC interrupt
     * service routine.  ((unsigned char *)framePtr->returnAddr)[-2]
     * is the operand specified for the SVC instruction. */

    unsigned char svc_operand;

    svc_operand = ((unsigned char *)framePtr->returnAddr)[-2];

#ifdef SVC_DEMO
void logSvcHandlerInC(struct frame *framePtr);
/* if we called svc_myFflush we might be ending the program */
/* and we'll have no opportunity to finish this logging so skip */
/* there must be a better way to do this */
if (svc_operand != SVC_FLUSHOUTPUT) {
    myFputs("Entering svcHandlerInC\r\n", STDOUT);
    logSvcHandlerInC(framePtr);
}
#endif

    switch(svc_operand) {
    case SVC_FREE:
        myFree((void *) framePtr->arg0);
        break;
    case SVC_MALLOC:
        framePtr->returnVal = (int) myMalloc((unsigned) framePtr->arg0);
        break;
    case SVC_FPUTC:
        framePtr->returnVal = myFputc(framePtr->arg0, (Stream *) framePtr->arg1);
        break;
    case SVC_FPUTS:
        framePtr->returnVal = myFputs((const char *) framePtr->arg0, (Stream *) framePtr->arg1);
        break;
    case SVC_FGETC:
        framePtr->returnVal = myFgetc((Stream *) framePtr->arg0);
        break;
    case SVC_FGETS:
        framePtr->returnVal = (int) myFgets((char *) framePtr->arg0, framePtr->arg1, (Stream *) framePtr->arg2);
        break;
    case SVC_FOPEN:
        framePtr->returnVal = (int) myFopen((const char *) framePtr->arg0);
        break;
    case SVC_FCLOSE:
        framePtr->returnVal = myFclose((Stream *) framePtr->arg0);
        break;
    case SVC_CREATE:
        framePtr->returnVal = myCreate((const char *) framePtr->arg0);
        break;
    case SVC_DELETE:
        framePtr->returnVal = myDelete((const char *) framePtr->arg0);
        break;
    case SVC_GET_CURRENT_MILLIS:
        framePtr->returnVal = get_current_millis();
        break;
    case SVC_GETTIMEOFDAY:
        framePtr->returnVal = gettimeofday((struct timeval *) framePtr->arg0, (void *) framePtr->arg1);
        break;
    case SVC_SETTIMEOFDAY:
        framePtr->returnVal = settimeofday((const struct timeval *) framePtr->arg0, (const struct timezone *) framePtr->arg1);
        break;
    case SVC_SETTIMER:
        setTimer((uint32_t) framePtr->arg0, (void (*)(void)) framePtr->arg1);
        break;
    case SVC_FLUSHOUTPUT:
        framePtr->returnVal = myFflush((Stream *) framePtr->arg0);
        break;
    case SVC_SPAWN:
        framePtr->returnVal = spawn_process((int (*)(int, char **)) framePtr->arg0);
        break;
    case SVC_YIELD:
        yield();
        break;
    case SVC_BLOCK:
        block();
        break;
    case SVC_WAKE:
        wake((uint32_t) framePtr->arg0);
        break;
    case SVC_KILL:
        myKill((uint32_t) framePtr->arg0);
        break;
    default:
        break;
    }

    #ifdef SVC_DEMO
    if (svc_operand != SVC_FLUSHOUTPUT) {
        myFputs("Exiting svcHandlerInC\r\n", STDOUT);
    }
    #endif
}

void logSvcHandlerInC(struct frame *framePtr) {
    char formatted_output[256];

    sprintf(formatted_output, "framePtr = 0x%08x\r\n", (unsigned int)framePtr);
    myFputs(formatted_output, STDOUT);

    sprintf(formatted_output, "SVC operand = %d\r\n",
            ((unsigned char *)framePtr->returnAddr)[-2]);
    myFputs(formatted_output, STDOUT);

    switch(((unsigned char *)framePtr->returnAddr)[-2]) {
    case SVC_FREE:
        myFputs("SVC FREE has been called\r\n", STDOUT);
        break;
    case SVC_MALLOC:
        myFputs("SVC MALLOC has been called\r\n", STDOUT);
        break;
    case SVC_FPUTC:
        myFputs("SVC FPUTC has been called\r\n", STDOUT);
        break;
    case SVC_FPUTS:
        myFputs("SVC FPUTS has been called\r\n", STDOUT);
        break;
    case SVC_FGETC:
        myFputs("SVC FGETC has been called\r\n", STDOUT);
        break;
    case SVC_FGETS:
        myFputs("SVC FGETS has been called\r\n", STDOUT);
        break;
    case SVC_FOPEN:
        myFputs("SVC FOPEN has been called\r\n", STDOUT);
        break;
    case SVC_FCLOSE:
        myFputs("SVC FCLOSE has been called\r\n", STDOUT);
        break;
    case SVC_CREATE:
        myFputs("SVC CREATE has been called\r\n", STDOUT);
        break;
    case SVC_DELETE:
        myFputs("SVC DELETE has been called\r\n", STDOUT);
        break;
    case SVC_GET_CURRENT_MILLIS:
        myFputs("SVC GET CURRENT MILLIS has been called\r\n", STDOUT);
        break;
    case SVC_GETTIMEOFDAY:
        myFputs("SVC GETTIMEOFDAY has been called\r\n", STDOUT);
        break;
    case SVC_SETTIMEOFDAY:
        myFputs("SVC SETTIMEOFDAY has been called\r\n", STDOUT);
        break;
    case SVC_SETTIMER:
        myFputs("SVC SETTIMER has been called\r\n", STDOUT);
        break;
    case SVC_FLUSHOUTPUT:
        myFputs("SVC FLUSHOUTPUT has been called\r\n", STDOUT);
        break;
    case SVC_SPAWN:
        myFputs("SVC SPAWN has been called\r\n", STDOUT);
        break;
    case SVC_YIELD:
        myFputs("SVC YIELD has been called\r\n", STDOUT);
        break;
    case SVC_BLOCK:
        myFputs("SVC BLOCK has been called\r\n", STDOUT);
        break;
    case SVC_WAKE:
        myFputs("SVC WAKE has been called\r\n", STDOUT);
        break;
    case SVC_KILL:
        myFputs("SVC KILL has been called\r\n", STDOUT);
        break;
    default:
        myFputs("Unknown SVC has been called\r\n", STDOUT);
        break;
    }
}
