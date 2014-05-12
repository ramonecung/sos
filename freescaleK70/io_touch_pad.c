#include "io.h"
#include "hardware/touch_pad.h"

void initialize_io_touch_pad(void) {
    TSI_Init();
    TSI_Calibrate();    
}

int fputc_touch_pad(int c, Stream *stream) {
    return c;
}

int fgetc_touch_pad(Stream *stream) {
    if (stream->device_instance == TOUCH_PAD_E1) {
        return electrode_in(ELECTRODE_NUMBER_E1);    
    } else if (stream->device_instance == TOUCH_PAD_E2) {
        return electrode_in(ELECTRODE_NUMBER_E2);    
    } else if (stream->device_instance == TOUCH_PAD_E3) {
        return electrode_in(ELECTRODE_NUMBER_E3);    
    } else if (stream->device_instance == TOUCH_PAD_E4) {
        return electrode_in(ELECTRODE_NUMBER_E4);    
    }
    return INVALID_TOUCH_PAD;
}
