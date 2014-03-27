#include "../include/constants.h"
#include "./RotateLED/led.h"
#include <stdlib.h>

#include "io.h"
#include "io_led.h"

Stream *fopen_led(void) {
    Stream *stream = malloc(sizeof(Stream));
    stream->device_instance = LED_ORANGE;
    Device *device = malloc(sizeof(Device));
    stream->device = device;
    return stream;
}

int fclose_led(Stream *stream) {
    free(stream->device);
    free(stream);
    return 0;
}

int fputc_led(int c, Stream *stream) {
    if (c == 0) {
        turn_off_led(stream->device_instance);
    } else {
        turn_on_led(stream->device_instance);
    }
    return c;
}

int fgetc_led(Stream *stream) {
    return 0;
}

void turn_on_led(enum device_instance di) {
    ;
}

void turn_off_led(enum device_instance di) {
    ;
}
