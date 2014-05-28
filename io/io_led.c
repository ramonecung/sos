#include "../include/constants.h"
#include "../util/util.h"
#include "../freescaleK70/hardware/led.h"
#include "../include/io.h"
#include "io_led.h"

void initialize_io_led(void) {
    ledInitAll();
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
    return EOF;
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
