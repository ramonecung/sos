#include "critical_section.h"

int count = 0;

void disable_interrupts(void) {
    count++;
    __asm("cpsid i");
}

void enable_interrupts(void) {
    count--;
    if (count <= 0) {
        __asm("cpsie i");
    }
}
