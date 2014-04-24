#include "io.h"
#ifdef SOS
#include "io_button.h"
#include "io_led.h"
#endif
#include "io_fs.h"
#include "../util/util.h"
#include "../util/strings.h"
#include <stdio.h>


void initialize_io(void) {
#ifdef SOS
    initialize_io_button();
    initialize_io_led();
#endif
    initialize_io_fs();
}

int myCreate(const char *filename) {
    return create_fs(filename);
}

int myDelete(const char *filename) {
    return delete_fs(filename);
}


Stream *myFopen(const char *filename) {
#ifdef SOS
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
    }
#endif
    return fopen_fs(filename);
}

int myFclose(Stream *stream) {
#ifdef SOS
    enum device_instance di = stream->device_instance;
    if (di == BUTTON_SW1 || di == BUTTON_SW2) {
        return fclose_button(stream);
    }
    if (di == LED_ORANGE || di == LED_YELLOW || di == LED_GREEN || di == LED_BLUE) {
        return fclose_led(stream);
    }
#endif
    return fclose_fs(stream);
}

int myFgetc(Stream *stream) {
#ifdef SOS
    enum device_instance di = stream->device_instance;
    if (di == BUTTON_SW1 || di == BUTTON_SW2) {
        return fgetc_button(stream);
    }
    if (di == LED_ORANGE || di == LED_YELLOW || di == LED_GREEN || di == LED_BLUE) {
        return fgetc_led();
    }
#endif
    return fgetc_fs(stream);
}

int myFputc(int c, Stream *stream) {
#ifdef SOS
    enum device_instance di = stream->device_instance;
    if (di == BUTTON_SW1 || di == BUTTON_SW2) {
        return fputc_button(c);
    }
    if (di == LED_ORANGE || di == LED_YELLOW || di == LED_GREEN || di == LED_BLUE) {
        return fputc_led(c, stream);
    }
#endif
    return fputc_fs(c, stream);
}

Stream *find_stream(enum device_instance di) {
#ifdef SOS
    if (di == BUTTON_SW1 || di == BUTTON_SW2) {
        return find_stream_button(di);
    }
    if (di == LED_ORANGE || di == LED_YELLOW || di == LED_GREEN || di == LED_BLUE) {
        return find_stream_led(di);
    }
#endif
    if (di >= FILE_SYSTEM_ID_START && di <= FILE_SYSTEM_ID_END) {
        return find_stream_fs(di);
    }
    return NULL_STREAM;
}
