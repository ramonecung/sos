#include "critical_section.h"

int disable_interrupts_count = 0;

void disable_interrupts(void) {
    __asm("cpsid i");
    disable_interrupts_count++;
}

void enable_interrupts(void) {
    disable_interrupts_count--;
    if (disable_interrupts_count <= 0) {
        __asm("cpsie i");
    }
}
