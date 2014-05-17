#include "init.h"
#include "../freescaleK70/hardware/mcg.h"
#include "../freescaleK70/hardware/sdram.h"
#include "../freescaleK70/hardware/lcdc.h"
#include "../freescaleK70/hardware/svc.h"
#include "../freescaleK70/hardware/flexTimer.h"
#include "../freescaleK70/hardware/priv.h"
#include "../freescaleK70/time.h"
#include "../freescaleK70/io.h"
#include "../memory/memory.h"

void initialize_system(void) {
    mcgInit();
    sdramInit();
    initialize_memory();
    initialize_io();
    /* Set the SVC handler priority */
    svcInit_SetSVCPriority(15);
    intFlexTimerInit();
    /* Allows interrupts (PRIMASK is cleared) -- this is also the default on reset */
    __asm("cpsie i");
    flexTimer0Start();
    privUnprivileged();
}
