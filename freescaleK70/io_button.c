#include "../include/constants.h"
#include "../util/util.h"
#include "hardware/pushbutton.h"
#include "io.h"
#include "io_button.h"

void initialize_io_button(void) {
    pushbuttonInitAll();
}

int fgetc_button(Stream *stream) {
    if (stream->device_instance == BUTTON_SW1) {
        return sw1In();
    }
    if (stream->device_instance == BUTTON_SW2) {
        return sw2In();
    }
    return INVALID_BUTTON;
}

int fputc_button(int c) {
    return c;
}
