#include "../include/constants.h"
#include "io.h"
#ifdef K70
#include "io_button.h"
#include "io_led.h"
#include "io_uart.h"
#include "io_lcd.h"
#include "io_adc.h"
#include "io_touch_pad.h"
#include "hardware/intSerialIO.h"
#endif
#include "io_fs.h"
#include "../memory/memory.h"
#include "../util/strings.h"

static Stream open_stream_head;
static Stream *OPEN_STREAM_HEAD;
static unsigned int STREAM_ID_SEQUENCE;

Stream *standard_input = NULL;
Stream *standard_output = NULL;
Stream *standard_error = NULL;

void initialize_io(void) {
    OPEN_STREAM_HEAD = &open_stream_head;
    OPEN_STREAM_HEAD->next = NULL;
    STREAM_ID_SEQUENCE = 0;
#ifdef K70
    initialize_io_button();
    initialize_io_led();
    initialize_io_uart();
    intSerialIOInit();
    initialize_standard_streams();
    initialize_io_lcd();
    initialize_io_adc();
    initialize_io_touch_pad();
#endif
    initialize_io_fs();
}

/* create bi-directional io stream for stdin, stdout, stderr */
void initialize_standard_streams(void) {
    standard_input = myFopen("/dev/uart/uart2");
    standard_output = myFopen("/dev/uart/uart2");
    standard_error = myFopen("/dev/uart/uart2");
}

int myCreate(const char *filename) {
    return create_fs(filename);
}

int myDelete(const char *filename) {
    return delete_fs(filename);
}


Stream *myFopen(const char *filename) {
    NamedFile *file; /* used for file system files */
    Stream *stream;
    stream = create_stream();
    if (stream == NULL) {
        return NULL;
    }

    stream->device_instance = device_instance_from_filename(filename);
    if (stream->device_instance == FILE_SYSTEM) {
        file = find_file(filename);
        if (file == NULL) {
            myFree(stream);
            return NULL;
        }
        setup_stream_fs(stream, file);
    }

    link_stream(stream);
    return stream;
}

Stream *create_stream(void) {
    Stream *stream;
    stream = myMalloc(sizeof(Stream));
    if (stream == NULL) {
        return NULL;
    }
    stream->stream_id = next_stream_id();
    stream->device_instance = (enum device_instance) NULL;
    stream->next = NULL;
    return stream;
}

enum device_instance device_instance_from_filename(const char *filename) {
#ifdef K70
    if (strings_equal(filename, "/dev/button/sw1")) {
        return BUTTON_SW1;
    } else if (strings_equal(filename, "/dev/button/sw2")) {
        return BUTTON_SW2;
    } else if (strings_equal(filename, "/dev/led/orange")) {
        return LED_ORANGE;
    } else if (strings_equal(filename, "/dev/led/yellow")) {
        return LED_YELLOW;
    } else if (strings_equal(filename, "/dev/led/green")) {
        return LED_GREEN;
    } else if (strings_equal(filename, "/dev/led/blue")) {
        return LED_BLUE;
    } else if (strings_equal(filename, "/dev/uart/uart2")) {
        return UART2;
    } else if (strings_equal(filename, "/dev/lcd/lcd")) {
        return LCD;
    } else if (strings_equal(filename, "/dev/adc/potentiometer")) {
        return POTENTIOMETER;
    } else if (strings_equal(filename, "/dev/adc/thermistor")) {
        return THERMISTOR;
    } else if (strings_equal(filename, "/dev/touch/e1")) {
        return TOUCH_PAD_E1;
    } else if (strings_equal(filename, "/dev/touch/e2")) {
        return TOUCH_PAD_E2;
    } else if (strings_equal(filename, "/dev/touch/e3")) {
        return TOUCH_PAD_E3;
    } else if (strings_equal(filename, "/dev/touch/e4")) {
        return TOUCH_PAD_E4;
    }
#endif
    return FILE_SYSTEM;
}

int myFclose(Stream *stream) {
    unlink_stream(stream);
    myFree((void *) stream);
    return SUCCESS;
}

unsigned int next_stream_id(void) {
    /* TODO: lock data */
    return STREAM_ID_SEQUENCE++;
}

void link_stream(Stream *stream) {
    stream->next = OPEN_STREAM_HEAD->next;
    OPEN_STREAM_HEAD->next = stream;
}

void unlink_stream(Stream *stream) {
    Stream *current, *previous;
    previous = OPEN_STREAM_HEAD;
    current = previous->next;
    while (current != NULL) {
        if (current->stream_id == stream->stream_id) {
            previous->next = current->next;
            break;
        }
        previous = current;
        current = current->next;
    }
}

Stream *find_stream(unsigned int stream_id) {
    Stream *current;
    current = OPEN_STREAM_HEAD->next;
    while (current != NULL) {
        if (current->stream_id == stream_id) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int myFgetc(Stream *stream) {
int c;
#ifdef K70
    if (stream == NULL) {
        return CANNOT_GET_CHAR;
    }
    if (stream_is_button(stream)) {
        return fgetc_button(stream);
    }
    if (stream_is_led(stream)) {
        return fgetc_led();
    }
    if (stream_is_uart(stream)) {
        /* UART should echo */
        c = fgetc_uart(stream);
        fputc_uart(c, stream);
        return c;
    }
    if (stream_is_lcd(stream)) {
        return fgetc_lcd(stream);
    }
    if (stream_is_potentiometer(stream)) {
        return fgetc_potentiometer(stream);
    }
    if (stream_is_thermistor(stream)) {
        return fgetc_thermistor(stream);
    }
    if (stream_is_touch_pad(stream)) {
        return fgetc_touch_pad(stream);
    }
#endif
    return fgetc_fs(stream);
}

char *myFgets(char *str, int size, Stream *stream) {
    char c;
    int i;

    if (size <= 0) {
        return NULL;
    }
    /* return NULL unless at least one character found */
    c = myFgetc(stream);
    if (c == EOF) {
        return NULL;
    }
    str[0] = c;
    /* storing size - 2 characters leaves room for \r\n */
    for (i = 1; i < size - 2; i++) {
        c = myFgetc(stream);
        if (c == CANNOT_GET_CHAR) {
            return NULL;
        }
        if (c == EOF) {
            break;
        }

        if (c == '\r' || c == '\n') {
            if (c == '\r') {
                str[i++] = '\r';
            }
            /* hack around the fact that Enter sends just a \r on the K70 */
            if (c == '\n' || stream_is_uart(stream)) {
                str[i++] = '\n';
            }
            break;
        } else {
            str[i] = c;
        }
    }
    str[i] = '\0';
    return str;
}

int myFputc(int c, Stream *stream) {
#ifdef K70
    if (stream_is_button(stream)) {
        return fputc_button(c);
    }
    if (stream_is_led(stream)) {
        return fputc_led(c, stream);
    }
    if (stream_is_uart(stream)) {
        return fputc_uart(c, stream);
    }
    if (stream_is_lcd(stream)) {
        return fputc_lcd(c, stream);
    }
    if (stream_is_potentiometer(stream)) {
        return fputc_potentiometer(c, stream);
    }
    if (stream_is_thermistor(stream)) {
        return fputc_thermistor(c, stream);
    }
    if (stream_is_touch_pad(stream)) {
        return fputc_touch_pad(c, stream);
    }
#endif
    return fputc_fs(c, stream);
}

int myFputs(const char *s, Stream *stream) {
    int res;
    char *cp = (char *) s;
    while (*cp) {
        res = myFputc(*cp++, stream);
        if (res == CANNOT_PUT_CHAR) {
            return EOF;
        }
    }
    return SUCCESS;
}

/* stream type matchers */
int stream_is_touch_pad(Stream *stream) {
    return device_is_touch_pad(stream->device_instance);
}

int stream_is_potentiometer(Stream *stream) {
    return device_is_potentiometer(stream->device_instance);
}

int stream_is_thermistor(Stream *stream) {
    return device_is_thermistor(stream->device_instance);
}

int stream_is_lcd(Stream *stream) {
    return device_is_lcd(stream->device_instance);
}

int stream_is_uart(Stream *stream) {
    return device_is_uart(stream->device_instance);
}

int stream_is_led(Stream *stream) {
    return device_is_led(stream->device_instance);
}

int stream_is_button(Stream *stream) {
    return device_is_button(stream->device_instance);
}

/* device type matchers */
int device_is_touch_pad(enum device_instance di) {
    return (di == TOUCH_PAD_E1 || di == TOUCH_PAD_E2 || di == TOUCH_PAD_E3 || di == TOUCH_PAD_E4);
}

int device_is_potentiometer(enum device_instance di) {
    return (di == POTENTIOMETER);
}

int device_is_thermistor(enum device_instance di) {
    return (di == THERMISTOR);
}

int device_is_lcd(enum device_instance di) {
    return (di == LCD);
}

int device_is_uart(enum device_instance di) {
    return (di == UART2);
}

int device_is_led(enum device_instance di) {
    return (di == LED_ORANGE || di == LED_YELLOW || di == LED_GREEN || di == LED_BLUE);
}

int device_is_button(enum device_instance di) {
    return (di == BUTTON_SW1 || di == BUTTON_SW2);
}
