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
        if (sw1In()) {
            /* wait until button released before checking if pushed again */
            while(sw1In()) { ; }
            return 1;
        }
        return 0;
    }
    if (stream->device_instance == BUTTON_SW2) {
        if (sw2In()) {
            /* wait until button released before checking if pushed again */
            while(sw2In()) { ; }
            return 1;
        }
        return 0;
    }
    return INVALID_BUTTON;
}

int fputc_button(int c) {
    return c;
}
