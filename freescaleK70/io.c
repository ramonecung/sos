#include "io.h"
#include "io_button.h"
#include "io_led.h"
#include "io_fs.h"
#include "../util/strings.h"

void initialize_io(void) {
    initialize_io_button();
    initialize_io_led();
}

Stream *myFopen(const char *filename) {
    if (strings_equal(filename, "/dev/button/sw1")) {
        return fopen_button(BUTTON_SW1);
    } else if (strings_equal(filename, "/dev/button/sw2")) {
        return fopen_button(BUTTON_SW2);
    } else if (strings_equal(filename, "/dev/led/orange")) {
        return fopen_led(LED_ORANGE);
    } else if (strings_equal(filename, "/dev/led/yellow")) {
        return fopen_led(LED_YELLOW);
    } else if (strings_equal(filename, "/dev/led/green")) {
        return fopen_led(LED_GREEN);
    } else if (strings_equal(filename, "/dev/led/blue")) {
        return fopen_led(LED_BLUE);
    } else if (filename_valid(filename)) {
        return fopen_fs();
    }
    return (Stream *) 0;
}

int myFclose(Stream *stream) {
    enum device_instance di = stream->device_instance;
    if (di == BUTTON_SW1 || di == BUTTON_SW2) {
        fclose_button(stream);
        return 0;
    }
    if (di == LED_ORANGE || di == LED_YELLOW || di == LED_GREEN || di == LED_BLUE) {
        fclose_led(stream);
        return 0;
    }
    return -1;
}

int myFgetc(Stream *stream) {
    enum device_instance di = stream->device_instance;
    if (di == BUTTON_SW1 || di == BUTTON_SW2) {
        return fgetc_button(stream);
    }
    if (di == LED_ORANGE || di == LED_YELLOW || di == LED_GREEN || di == LED_BLUE) {
        return fgetc_led();
    }
    return -1;
}

int myFputc(int c, Stream *stream) {
    enum device_instance di = stream->device_instance;
    if (di == BUTTON_SW1 || di == BUTTON_SW2) {
        return fputc_button(c);
    }
    if (di == LED_ORANGE || di == LED_YELLOW || di == LED_GREEN || di == LED_BLUE) {
        return fputc_led(c, stream);
    }
    return -1;
}
