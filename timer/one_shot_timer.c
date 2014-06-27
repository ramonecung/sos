#include "one_shot_timer.h"
#include "../freescaleK70/PDB.h"

void (*action)(void);

void setTimer(uint16_t count,  void (*function)(void)) {
    action = function;
    /* set interval to count 1/46875 second periods. 46875 for 1 second */
    PDB0Init(count, PDBTimerOneShot);
    PDB0Start();
}

void timerAction(void) {
    action();
}
