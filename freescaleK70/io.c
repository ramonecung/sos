#include "io.h"
#include "io_button.h"
#include "../util/strings.h"

Stream *myFopen(const char *filename) {
    if (strings_equal(filename, "BUTTON_SW1")) {
        return fopen_button(BUTTON_SW1);
    } else if (strings_equal(filename, "LED_ORANGE")) {
        return fopen_button(LED_ORANGE);
    }
    return fopen_button(BUTTON_SW1);
}

int myFclose(Stream *stream) {
    if (stream->device_instance == BUTTON_SW1) {
        fclose_button(stream);
    }
    return 0;
}

