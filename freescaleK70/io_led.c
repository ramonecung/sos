#include "../include/constants.h"
#include "./RotateLED/led.h"
#include <stdlib.h>

#include "io.h"
#include "io_led.h"

void initialize_io_led(void) {
    ledInitAll();
}

Stream *fopen_led(void) {
    Stream *stream;
    Device *device;
    stream = malloc(sizeof(Stream));
    stream->device_instance = LED_ORANGE;
    device = malloc(sizeof(Device));
    stream->device = device;
    return stream;
}

int fclose_led(Stream *stream) {
    free(stream->device);
    free(stream);
    /* go backwards on ledInitAll */
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

int fgetc_led(void) {
    return 0;
}

void turn_on_led(enum device_instance di) {
    if (di == LED_ORANGE) {
        ledOrangeOn();
    }
}

void turn_off_led(enum device_instance di) {
    if (di == LED_ORANGE) {
        ledOrangeOff();
    }
}
