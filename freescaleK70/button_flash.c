#include "button_flash.h"

static int number_presses;

void bfInitialize(void) {
    number_presses = 0;
}

void button_press(void) {
    number_presses++;
    if (number_presses == 4) {
        number_presses = 0;
    }
}

int orange_status(void) {
    if (number_presses == 0) {
        return ON;
    }
    return OFF;
}

int yellow_status(void) {
    if (number_presses == 1) {
        return ON;
    }
    return OFF;
}

int green_status(void) {
    if (number_presses == 2) {
        return ON;
    }
    return OFF;
}

int blue_status(void) {
    if (number_presses == 3) {
        return ON;
    }
    return OFF;
}

