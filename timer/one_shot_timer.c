#include "one_shot_timer.h"
#include "../freescaleK70/hardware/PDB.h"

void setTimer(uint16_t count) {
    /* set interval to count 1/60,000,000 second periods. 60000000 for 1 second */
    PDB0Init(count, PDBTimerOneShot);
    PDB0Start();
}
