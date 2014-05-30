#include "init.h"
#include "../freescaleK70/mcg.h"
#include "../freescaleK70/sdram.h"
#include "../freescaleK70/lcdc.h"
#include "../include/svc.h"
#include "../freescaleK70/flexTimer.h"
#include "../freescaleK70/priv.h"
#include "../include/io.h"
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
