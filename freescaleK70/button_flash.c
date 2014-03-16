#include "button_flash.h"

static int number_presses;

void bfInitialize(void) {
    number_presses = 0;
}

int orange_status(void) {
    if (number_presses == 1) {
        return OFF;
    }
    return ON;
}

int yellow_status(void) {
    if (number_presses == 1) {
        return ON;
    }
    return OFF;
}

int green_status(void) {
    return OFF;
}

int blue_status(void) {
    return OFF;
}

void button_press(void) {
    number_presses++;
}

