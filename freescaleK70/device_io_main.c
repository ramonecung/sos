#include "derivative.h" /* include peripheral declarations */

#include "io.h"
#include "hardware/delay.h"
#include <stdio.h>

void fputc_led_demo(char *led) {
    Stream *stream;
    stream = myFopen(led);
    myFputc(1, stream);
    delay(1000000);
    myFputc(0, stream);
    myFclose(stream);
}

void fgetc_button_demo(void) {
    Stream *stream1 = myFopen("/dev/button/sw1");
    Stream *stream2 = myFopen("/dev/button/sw2");
    int counter = 0;
    int sw1, sw2;
    while(1) {
        sw1 = myFgetc(stream1);
        if (sw1) {
            counter++;
            printf("SW1 pressed\n");
        }
        sw2 = myFgetc(stream2);
        if (sw2) {
            counter++;
            printf("SW2 pressed\n");
        }
        if (counter >= 6) {
            myFclose(stream1);
            myFclose(stream2);
            break;
        }
    }
}

int main(void) {
    initialize_io();
    fputc_led_demo("/dev/led/orange");
    fputc_led_demo("/dev/led/yellow");
    fputc_led_demo("/dev/led/green");
    fputc_led_demo("/dev/led/blue");

    fgetc_button_demo();

    return 0;
}
