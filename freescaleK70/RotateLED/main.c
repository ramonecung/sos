/**
 * main.c
 * flashes the four LEDs in response to button press of SW1
 */

#include "../hardware/led.h"
#include "../hardware/pushbutton.h"
#include "../hardware/delay.h"

#define DELAY_LIMIT 500000

int main(void) {
    int number_presses = 0;

    /* Initialize all of the LEDs */
    ledInitAll();
    /* Initialize both of the pushbuttons */
    pushbuttonInitAll();

    /* initially orange LED should be on and all others should be off */
    ledOrangeOn();

    while(1) {
        if (sw1In()) {
            /* avoid contact bounce */
            delay(DELAY_LIMIT);

            number_presses++;
            if (number_presses == 4) {
                number_presses = 0;
            }

            if (number_presses == 0) {
                ledOrangeOn();
            } else {
                ledOrangeOff();
            }

            if (number_presses == 1) {
                ledYellowOn();
            } else {
                ledYellowOff();
            }

            if (number_presses == 2) {
                ledGreenOn();
            } else {
                ledGreenOff();
            }

            if (number_presses == 3) {
                ledBlueOn();
            } else {
                ledBlueOff();
            }

            /* wait until button released before checking if pushed again */
            while(sw1In()) { ; }
        }
    }

    return 0;
}

