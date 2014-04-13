#include "../include/constants.h"
#include "./RotateLED/pushbutton.h"
#include <stdlib.h>

#include "io.h"
#include "io_button.h"

void initialize_io_button(void) {
    pushbuttonInitAll();
}

Stream *fopen_button(enum device_instance di) {
    Stream *stream;
    Device *device;
    stream = malloc(sizeof(Stream));
    device = malloc(sizeof(Device));
    stream->device = device;
    stream->device_instance = di;
    return stream;
}

int fclose_button(Stream *stream) {
    free(stream->device);
    free(stream);
    return 0;
}

int fgetc_button(Stream *stream) {
    if (stream->device_instance == BUTTON_SW1) {
        return sw1In();
    }
    if (stream->device_instance == BUTTON_SW2) {
        return sw2In();
    }
    return -1;
}

int fputc_button(int c) {
    return c;
}
