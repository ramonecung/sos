#include "io.h"
#include "io_button.h"
#include "io_led.h"
#include "io_fs.h"
#include "../util/util.h"
#include "../util/strings.h"
#include <stdio.h>

void initialize_io(void) {
    Stream *null_stream;
    Device *null_device;
    initialize_io_button();
    initialize_io_led();

    null_stream = (Stream *) emalloc(sizeof(Stream), "initialize_io", stderr);
    null_device = (Device *) emalloc(sizeof(Device), "initialize_io", stderr);
    null_stream->device_instance = NULL_DEVICE;
    null_stream->device = null_device;
    NULL_STREAM = null_stream;
}

int myCreate(const char *filename) {
    return create_fs(filename);
}

int myDelete(const char *filename) {
    return delete_fs(filename);
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
    } else {
        return fopen_fs(filename);
    }
}

int myFclose(Stream *stream) {
    enum device_instance di = stream->device_instance;
    if (di == BUTTON_SW1 || di == BUTTON_SW2) {
        return fclose_button(stream);
    }
    if (di == LED_ORANGE || di == LED_YELLOW || di == LED_GREEN || di == LED_BLUE) {
        return fclose_led(stream);
    }
    return fclose_fs(stream);
}

int myFgetc(Stream *stream) {
    enum device_instance di = stream->device_instance;
    if (di == BUTTON_SW1 || di == BUTTON_SW2) {
        return fgetc_button(stream);
    }
    if (di == LED_ORANGE || di == LED_YELLOW || di == LED_GREEN || di == LED_BLUE) {
        return fgetc_led();
    }
    return fgetc_fs(stream);
}

int myFputc(int c, Stream *stream) {
    enum device_instance di = stream->device_instance;
    if (di == BUTTON_SW1 || di == BUTTON_SW2) {
        return fputc_button(c);
    }
    if (di == LED_ORANGE || di == LED_YELLOW || di == LED_GREEN || di == LED_BLUE) {
        return fputc_led(c, stream);
    }
    return fputc_fs(c, stream);
}
