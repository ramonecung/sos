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
