#include "init.h"
#include "../freescaleK70/hardware/mcg.h"
#include "../freescaleK70/hardware/sdram.h"

void initialize_system(void) {
    mcgInit();
    sdramInit();
}
