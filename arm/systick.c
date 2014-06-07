#include <derivative.h>
#include "systick.h"

/* This function sets the priority at which the systick handler runs (See
 * B3.2.11, System Handler Priority Register 3, SHPR3 on page B3-724 of
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
 * explicitly mention the System Handler (like the Systick handler) priorities,
 * but they should be handled consistently with the interrupt priorities.
 */

void systickInit_SetSystickPriority(unsigned char priority) {
    if(priority > Systick_MaxPriority)
        return;

    SCB_SHPR3 = (SCB_SHPR3 & ~SCB_SHPR3_PRI_15_MASK) |
            SCB_SHPR3_PRI_15(priority << Systick_PriorityShift);
}

void systickIsr(void) {
    return;
}

void pendSVIsr(void) {
    return;
}
