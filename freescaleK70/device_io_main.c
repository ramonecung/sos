
#include "../include/constants.h"
#include "io_constants.h"
#include "io.h"
#include "hardware/delay.h"
#include "../memory/memory.h"

#include <stdio.h>

void fputc_led_demo(char *led) {
    Stream *stream;
    stream = myFopen(led);
    printf("Turn on %s\n", led);
    myFputc(1, stream);
    delay(1000000);
    printf("Turn off %s\n", led);
    myFputc(0, stream);
    myFclose(stream);
}

void fgetc_button_demo(void) {
    printf("A total of 6 button presses will terminate program\n");
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

void fs_demo(void) {
    Stream *stream;
    int a, b, c;

    printf("Create /dev/fs/text...\n");
    myCreate("/dev/fs/text");

    printf("Open a stream on /dev/fs/text...\n");
    stream = myFopen("/dev/fs/text");

    printf("Write characters a, b, c to the stream...\n");
    myFputc('a', stream);
    myFputc('b', stream);
    myFputc('c', stream);

    printf("Read a character and print it\n");
    a = myFgetc(stream);
    printf("%c\n", a);

    printf("Read a character and print it\n");
    b = myFgetc(stream);
    printf("%c\n", b);

    printf("Read a character and print it\n");
    c = myFgetc(stream);
    printf("%c\n", c);

    printf("Close the stream\n");
    myFclose(stream);

    printf("Delete /dev/fs/text\n");
    myDelete("/dev/fs/text");
}

int main(void) {
    initialize_memory();
    initialize_io();

    printf("In memory file-system:\n");
    fs_demo();

    printf("LEDs:\n");
    fputc_led_demo("/dev/led/orange");
    fputc_led_demo("/dev/led/yellow");
    fputc_led_demo("/dev/led/green");
    fputc_led_demo("/dev/led/blue");

    printf("Buttons:\n");
    fgetc_button_demo();

    printf("Goodbye\n");
    return 0;
}
