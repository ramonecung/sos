#include "init.h"
#include "../freescaleK70/mcg.h"
#include "../freescaleK70/sdram.h"
#include "../freescaleK70/lcdc.h"
#include "../include/svc.h"
#include "../arm/systick.h"
#include "../freescaleK70/flexTimer.h"
#include "../freescaleK70/priv.h"
#include "../include/io.h"
#include "../memory/memory.h"

void initialize_system(void) {
    mcgInit();           /* hardware clocks */
    sdramInit();         /* memory hardware */
    initialize_memory(); /* system memory manager */
    initialize_io();     /* device and file system I/O */
    svcInit();           /* supervisor call interface */
    intFlexTimerInit();  /* for maintaining system time */
/*
 * set up env
initialize all devices besides systick timer
malloc storage for brand new stack
take address of something just past the stack and make stack pointer point at it
enable systick timer
 */
    inititialize_process_manager(); /* process manager and base process */
    systickInit();                  /* for process quantum interrupts */
    privUnprivileged();             /* start running in user mode */
}
