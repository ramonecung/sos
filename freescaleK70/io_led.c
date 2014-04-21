#include "../include/constants.h"
#include "../util/util.h"
#include "hardware/led.h"
#include <stdlib.h>

#include "io.h"
#include "io_led.h"

static Stream *open_led_files[NUMBER_LEDS];

void initialize_io_led(void) {
    int i;
    ledInitAll();
    for (i = 0; i < NUMBER_LEDS; i++) {
        open_led_files[i] = NULL_STREAM;
    }
}

Stream *fopen_led(enum device_instance di) {
    Stream *stream;
    Device *device;
    int open_file_index;
    stream = emalloc(sizeof(Stream), "fopen_led", stderr);
    stream->device_instance = di;
    device = emalloc(sizeof(Device), "fopen_led", stderr);
    stream->device = device;

    open_file_index = ((int) di) - LED_DEVICE_ID_START;
    /* close the led if it is already open */
    /* i.e. it can only be open once */
    if (open_led_files[open_file_index] != NULL_STREAM) {
        fclose_led(open_led_files[open_file_index]);
    }
    open_led_files[open_file_index] = stream;

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

int fgetc_led(void) {
    return 0;
}

void turn_on_led(enum device_instance di) {
    if (di == LED_ORANGE) {
        ledOrangeOn();
    } else if (di == LED_YELLOW) {
        ledYellowOn();
    } else if (di == LED_GREEN) {
        ledGreenOn();
    } else if (di == LED_BLUE) {
        ledBlueOn();
    }
}

void turn_off_led(enum device_instance di) {
    if (di == LED_ORANGE) {
        ledOrangeOff();
    } else if (di == LED_YELLOW) {
        ledYellowOff();
    } else if (di == LED_GREEN) {
        ledGreenOff();
    } else if (di == LED_BLUE) {
        ledBlueOff();
    }
}
