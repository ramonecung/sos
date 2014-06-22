#include <derivative.h>
#include "systick.h"
#include "../include/svc.h"
#include "../process/process.h"

/* This function sets the priority at which the systick handler runs (See
 * B3.2.11, System Handler Priority Register 3, SHPR3 on page B3-724 of
 * the ARM v7-M Architecture Reference Manual, ARM DDI 0403Derrata
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
 * explicitly mention the System Handler (like the Systick handler) priorities,
 * but they should be handled consistently with the interrupt priorities.
 */

void systickInit_SetSystickPriority(unsigned char priority) {
    if(priority > Systick_MaxPriority)
        return;

    SCB_SHPR3 = (SCB_SHPR3 & ~SCB_SHPR3_PRI_15_MASK) |
            SCB_SHPR3_PRI_15(priority << Systick_PriorityShift);
}


/*
The system timer, SysTick See ARM pg 744

We also have a couple of K70 restrictions noted in the K70 Sub-Family
Reference Manual in section 3.2.1.2 "System Tick Timer" on page 83.
This implies that you must select the Processor Clock (using CLKSOURCE
in SYST_CSR.  Once doing so, when the SysTick is enabled, the SysTick
timer will count down once per processor clock cycle.  After calling
mcgInit(), the processor clock is set to 120 MHz and, as a result,
SysTick will be decremented once every 1/120,000,000 seconds, or
8.3333 x 10^(-9), or 8.3333 nanoseconds. Because the largest 24-bit
reset value (stored in SYST_RVR) is 2^24, or 16,777,216, that implies
that the maximum period for interrupts from the SysTick timer is
16,777,216 x 8.3333 nanoseconds, or 139.81013333 milliseconds.
Therefore, it should be clear that this timer supports all reasonable
quantum interrupt periods.
*/
void systickInit(void) {
    systickInit_SetSystickPriority(Systick_Priority);

    /* must use the processor clock - this might always be set on the K70 */
    SYST_CSR |= SysTick_CSR_CLKSOURCE_MASK;

    /*
     * Before enabling the SysTick counter, software must
     * write the required counter value to SYST_RVR, and then write to SYST_CVR.
     * This clears SYST_CVR to zero. When enabled, the counter reloads
     * the value from SYST_RVR, and counts down from that value,
     * rather than from an arbitrary value.
     */
    SYST_RVR |= SysTick_RVR_RELOAD(PROCESS_QUANTUM);
    /* Any write to the register clears the register to zero. */
    SYST_CVR = 0;

    /* 1 = Count to 0 changes the SysTick exception status to pending. */
    SYST_CSR |= SysTick_CSR_TICKINT_MASK;

    /* enable the counter */
    SYST_CSR |= SysTick_CSR_ENABLE_MASK;
}

#ifdef __GNUC__
void systickIsr(void) {
    struct PCB *current, *next;
    current = get_current_process();
    next = choose_process_to_run();

    /*
        The state of the process will include
        all registers that processes are allowed to change:
        R0, R1, R2, R3, R12, SP (R13), LR (R14), xPSR, PC (R15),
        the remaining registers:
        R4 through R11,
        and the stack.

        When acknowledging the interrupt the processor will push
        R0, R1, R2, R3, R12, SP (R13), LR (R14), xPSR, PC (R15)
    */
    __asm("push {r4,r5,r6,r8,r9,r10,r11}");

    /* push SVC state */
    __asm("ldr  r0, [%[shcsr]]"   "\n"
        "and  r0, r0, %[mask]"  "\n"
        "push {r0}"
        :
        : [shcsr] "r" (&SCB_SHCSR), [mask] "I" (SCB_SHCSR_SVCALLACT_MASK)
        : "r0", "memory", "sp");

    /* The following assembly language will put the current main SP
       value into the PCB's saved stack pointer */
    __asm("mrs %[mspDest],msp" : [mspDest]"=r"(current->stack_pointer));

    /* pause process */
    current->total_cpu_time += PROCESS_QUANTUM;
    /* might have already updated state to KILLED or BLOCKED upstream */
    if (current->state == RUNNING) {
        current->state = READY;
    }
    /* The following assembly language will write the value of the
       the PCB's saved stack pointer into the main SP */
    __asm("msr msp,%[mspSource]" : : [mspSource]"r"(next->stack_pointer) : "sp");

    /* pop SVC state */
    __asm("pop {r0}"              "\n"
        "ldr r1, [%[shcsr]]"    "\n"
        "bic r1, r1, %[mask]"   "\n"
        "orr r0, r0, r1"        "\n"
        "str r0, [%[shcsr]]"
        :
        : [shcsr] "r" (&SCB_SHCSR), [mask] "I" (SCB_SHCSR_SVCALLACT_MASK)
        : "r0", "r1", "sp", "memory");


    __asm("pop {r4,r5,r6,r8,r9,r10,r11}");

    next->state = RUNNING;
    set_current_process(next);

    /* the exit code will use r7 as the reference for the stack pointer */
    /* because it left the value of SP for the old process there */
    /* need to switch r7 to the new process's stack pointer */
    /* that we just started using, accounting for the pops we did */
    __asm("mov r7, %[nextSP]" : : [nextSP] "r" (next->stack_pointer + 8));
}
#endif

