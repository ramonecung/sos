#include "init.h"
#include "../freescaleK70/mcg.h"
#include "../freescaleK70/sdram.h"
#include "../freescaleK70/lcdc.h"
#include "../include/svc.h"
#include "../arm/systick.h"
#include "../arm/pendsv.h"
#include "../freescaleK70/flexTimer.h"
#include "../freescaleK70/priv.h"
#include "../include/io.h"
#include "../memory/memory.h"
#include "../process/process.h"

void initialize_system(void) {
	/*
	 * set up environment
	 * initialize all devices besides systick timer
	 * malloc storage for brand new stack (done within initialize_process_manager)
	 * make stack pointer point at it
	 * enable systick timer
	 */
	struct PCB *init_process;
	uint32_t pushed_reg;

    mcgInit();           /* hardware clocks */
    sdramInit();         /* memory hardware */
    initialize_memory(); /* system memory manager */
    initialize_io();     /* device and file system I/O */
    svcInit();           /* supervisor call interface */
    pendSVInit();        /* PendSV interrupt priority */
    intFlexTimerInit();  /* for maintaining system time */

    initialize_process_manager();   /* process manager and base process */
    /* this function's entry code pushes R4, R7 and LR */
    /* and decreases SP by 12 bytes (3 words) */
    /* need those pushed R4, R7, and LR values on the init process's stack */
    /* we can retrieve the pushed values of R4, R7 and LR */
    /* using offsets of 12, 16, and 20 bytes from the current SP */
    init_process = get_current_process();

    __asm("ldr %[pushedLR], [sp, #20]" : [pushedLR] "=r" (pushed_reg));
    __asm("add %[dest], %[pushedLR], #0" : [dest] "=r" (*(--init_process->stack_pointer)) : [pushedLR] "r" (pushed_reg));

    __asm("ldr %[pushedR7], [sp, #16]" : [pushedR7] "=r" (pushed_reg));
    __asm("add %[dest], %[pushedR7], #0" : [dest] "=r" (*(--init_process->stack_pointer)) : [pushedR7] "r" (pushed_reg));

    __asm("ldr %[pushedR4], [sp, #12]" : [pushedR4] "=r" (pushed_reg));
    __asm("add %[dest], %[pushedR4], #0" : [dest] "=r" (*(--init_process->stack_pointer)) : [pushedR4] "r" (pushed_reg));

    /* the entry code stores SP in R7 after decrementing it by 3 words */
    /* on exit, R7 is incremented by 3 words, and then moved into SP */
    /* so we need R7 to be 3 words below the init process's stack pointer */
    __asm("mov r7, %[initSP]" : : [initSP] "r" (init_process->stack_pointer - 3));

    /* move the SP to the simulated top of the init process's stack */
    __asm("msr msp, r7" : : : "sp");

    systickInit();                  /* for process quantum interrupts */
    privUnprivileged();             /* start running in user mode */
}
