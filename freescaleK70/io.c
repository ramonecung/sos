#include "../include/constants.h"
#include "io.h"
#ifdef SOS
#include "io_button.h"
#include "io_led.h"
#endif
#include "io_fs.h"
#include "../util/util.h"
#include "../util/strings.h"

static Stream open_stream_head;
static Stream *OPEN_STREAM_HEAD;
static unsigned int STREAM_ID_SEQUENCE;

void initialize_io(void) {
    OPEN_STREAM_HEAD = &open_stream_head;
    OPEN_STREAM_HEAD->next = NULL;
    STREAM_ID_SEQUENCE = 0;
#ifdef SOS
    initialize_io_button();
    initialize_io_led();
#endif
    initialize_io_fs();

    /* create bi-directional io stream for stdin, stdout, stderr */
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
            efree(stream);
            return NULL;
        }
        setup_stream_fs(stream, file);
    }

    link_stream(stream);
    return stream;
}

enum device_instance device_instance_from_filename(const char *filename) {
#ifdef SOS
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
    }
#endif
    return FILE_SYSTEM;
}

int myFclose(Stream *stream) {
    unlink_stream(stream);
    efree((void *) stream);
    return SUCCESS;
}

Stream *create_stream(void) {
    Stream *stream;
    stream = emalloc(sizeof(Stream), "create_stream", stderr);
    if (stream == NULL) {
        return NULL;
    }
    stream->stream_id = next_stream_id();
    stream->device_instance = (enum device_instance) NULL;
    stream->next = NULL;
    return stream;
}

unsigned int next_stream_id(void) {
    /* TODO: lock data */
    return STREAM_ID_SEQUENCE++;
}

void link_stream(Stream *stream) {
    Stream *current, *previous;
    previous = current = OPEN_STREAM_HEAD;
    while (current->next != NULL) {
        previous = current;
        current = current->next;
    }
    previous->next = stream;
}

void unlink_stream(Stream *stream) {
    Stream *current, *previous;
    previous = OPEN_STREAM_HEAD;
    current = previous->next;
    while (current != NULL) {
        if (current->stream_id == stream->stream_id) {
            previous->next = current->next;
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
#ifdef SOS
    if (stream == NULL) {
        return CANNOT_GET_CHAR;
    }
    if (stream_is_button(stream)) {
        return fgetc_button(stream);
    }
    if (stream_is_led(stream)) {
        return fgetc_led();
    }
#endif
    return fgetc_fs(stream);
}

int myFputc(int c, Stream *stream) {
#ifdef SOS
    if (stream_is_button(stream)) {
        return fputc_button(c);
    }
    if (stream_is_led(stream)) {
        return fputc_led(c, stream);
    }
#endif
    return fputc_fs(c, stream);
}


int stream_is_led(Stream *stream) {
    return device_is_led(stream->device_instance);
}

int stream_is_button(Stream *stream) {
    return device_is_button(stream->device_instance);
}

int device_is_led(enum device_instance di) {
    return (di == LED_ORANGE || di == LED_YELLOW || di == LED_GREEN || di == LED_BLUE);
}

int device_is_button(enum device_instance di) {
    return (di == BUTTON_SW1 || di == BUTTON_SW2);
}
