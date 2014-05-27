#include "init.h"
#include "../freescaleK70/hardware/mcg.h"
#include "../freescaleK70/hardware/sdram.h"
#include "../freescaleK70/hardware/lcdc.h"
#include "../include/svc.h"
#include "../freescaleK70/hardware/flexTimer.h"
#include "../freescaleK70/hardware/priv.h"
#include "../freescaleK70/io.h"
#include "../memory/memory.h"

void initialize_system(void) {
    mcgInit();
    sdramInit();
    initialize_memory();
    initialize_io();
    svcInit_SetSVCPriority(15); /* Set the SVC handler priority */
    intFlexTimerInit();
    privUnprivileged();
}
