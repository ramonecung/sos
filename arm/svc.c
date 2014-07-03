#include "derivative.h"
#include "../time/time.h"

#include "../include/svc.h"
#include "critical_section.h"
#include "svc_internal.h"
#include "../process/process.h"

//static int already_handling_svc = 0;

static void down(void) {
//    while (1) {
//        disable_interrupts();
//        if (!already_handling_svc) {
//            already_handling_svc = 1;
//            break;
//        }
//        enable_interrupts();
//        svc_block();
//    }
//    enable_interrupts();
}

static void up(void) {
//    disable_interrupts();
//    wake_all();
//    already_handling_svc = 0;
//    enable_interrupts();
}


void svc_myFree(void *ptr) {
    down();
    internal_svc_myFree(ptr);
    up();
}

void *svc_myMalloc(unsigned int size) {
    void *ptr;
    down();
    ptr = internal_svc_myMalloc(size);
    up();
    return ptr;
}

int svc_myFputc(int c, Stream *stream) {
    int d;
    down();
    d = internal_svc_myFputc(c, stream);
    up();
    return d;
}

int svc_myFputs(const char *s, Stream *stream) {
    int result;
    down();
    result = internal_svc_myFputs(s, stream);
    up();
    return result;
}

int svc_myFflush(Stream *stream) {
    int result;
    down();
    result = internal_svc_myFflush(stream);
    up();
    return result;
}

int svc_myFgetc(Stream *stream) {
    int c;
    down();
    c = internal_svc_myFgetc(stream);
    up();
    return c;
}

char *svc_myFgets(char *str, int size, Stream *stream) {
    down();
    str = internal_svc_myFgets(str, size, stream);
    up();
    return str;
}

Stream *svc_myFopen(const char *filename) {
    Stream *s;
    down();
    s = internal_svc_myFopen(filename);
    up();
    return s;
}

int svc_myFclose(Stream *stream) {
    int result;
    down();
    result = internal_svc_myFclose(stream);
    up();
    return result;
}

int svc_myCreate(const char *filename) {
    int result;
    down();
    result = internal_svc_myCreate(filename);
    up();
    return result;
}

int svc_myDelete(const char *filename) {
    int result;
    down();
    result = internal_svc_myDelete(filename);
    up();
    return result;
}

uint64_t svc_get_current_millis(void) {
    uint64_t current_millis;
    down();
    current_millis = internal_svc_get_current_millis();
    up();
    return current_millis;
}

int svc_gettimeofday(struct timeval *tp, void *tzp) {
    int result;
    down();
    result = internal_svc_gettimeofday(tp, tzp);
    up();
    return result;
}

int svc_settimeofday(const struct timeval *tp, const struct timezone *tzp) {
    int result;
    down();
    result = internal_svc_settimeofday(tp, tzp);
    up();
    return result;
}

void svc_setTimer(uint32_t count, void (*function)(void)) {
    down();
    internal_svc_setTimer(count, function);
    up();
}

int svc_spawn(int (*mainfunc)(int argc, char **argv)) {
    int pid;
    down();
    pid = internal_svc_spawn(mainfunc);
    up();
    return pid;
}

void svc_yield(void) {
    down();
    internal_svc_yield();
    up();
}

void svc_block(void) {
    down();
    internal_svc_block();
    up();
}

void svc_wake(uint32_t pid) {
    down();
    internal_svc_wake(pid);
    up();
}

void svc_myKill(uint32_t pid) {
    down();
    internal_svc_myKill(pid);
    up();
}


/* This function sets the priority at which the SVCall handler runs (See
 * B3.2.11, System Handler Priority Register 2, SHPR2 on page B3-723 of
 * the ARM®v7-M Architecture Reference Manual, ARM DDI 0403Derrata
 * 2010_Q3 (ID100710)).
 *
 * If priority parameter is invalid, this function performs no action.
 *
 * The ARMv7-M Architecture Reference Manual in section "B1.5.4 Exception
 * priorities and preemption" on page B1-635 states, "The number of
 * supported priority values is an IMPLEMENTATION DEFINED power of two in
 * the range 8 to 256, and the minimum supported priority value is always 0.
 * All priority value fields are 8-bits, and if an implementation supports
 * fewer than 256 priority levels then low-order bits of these fields are RAZ."
 *
 * In the K70 Sub-Family Reference Manual in section "3.2.2.1 Interrupt
 * priority levels" on page 85, it states, "This device supports 16 priority
 * levels for interrupts.  Therefore, in the NVIC each source in the IPR
 * registers contains 4 bits."  The diagram that follows goes on to confirm
 * that only the high-order 4 bits of each 8 bit field is used.  It doesn't
 * explicitly mention the System Handler (like the SVC handler) priorities,
 * but they should be handled consistently with the interrupt priorities.
 *
 * It is important to set the SVCall priority appropriately to allow
 * or disallow interrupts while the SVCall handler is running. */

void svcInit_SetSVCPriority(unsigned char priority) {
    if(priority > SVC_MaxPriority)
        return;

    SCB_SHPR2 = (SCB_SHPR2 & ~SCB_SHPR2_PRI_11_MASK) |
            SCB_SHPR2_PRI_11(priority << SVC_PriorityShift);
}

void svcInit(void) {
    svcInit_SetSVCPriority(SVC_Priority);
}
