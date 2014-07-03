#include "derivative.h"
#include "pendsv.h"

void svcInit_SetPendSVPriority(unsigned char priority);

void pendSVInit(void) {
    svcInit_SetPendSVPriority(PendSV_Priority);
}

void svcInit_SetPendSVPriority(unsigned char priority) {
    if(priority > PendSV_MaxPriority)
        return;

    SCB_SHPR3 = (SCB_SHPR3 & ~SCB_SHPR3_PRI_14_MASK) |
            SCB_SHPR3_PRI_14(priority << PendSV_PriorityShift);
}
