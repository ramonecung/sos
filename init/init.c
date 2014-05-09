#include "init.h"
#include "../freescaleK70/hardware/mcg.h"
#include "../freescaleK70/hardware/sdram.h"
#include "../freescaleK70/io.h"
#include "../memory/memory.h"

int SYSTEM_INITIALIZED = 0;

void initialize_system(void) {
    mcgInit();
    sdramInit();
    initialize_memory();
    initialize_io();
    SYSTEM_INITIALIZED = 1;
}

int system_initialized(void) {
	return SYSTEM_INITIALIZED;
}
