#include "../include/constants.h"
#include "hardware/pushbutton.h"
#include <stdlib.h>

#include "io.h"
#include "io_button.h"

static Stream *open_button_files[NUMBER_BUTTONS];

void initialize_io_button(void) {
    pushbuttonInitAll();
}

Stream *find_stream_button(enum device_instance di) {
    int open_file_index;
    open_file_index = ((int) di) - BUTTON_DEVICE_ID_START;
    return open_button_files[open_file_index];
}

Stream *fopen_button(enum device_instance di) {
    Stream *stream;
    Device *device;
    int open_file_index;
    stream = malloc(sizeof(Stream));
    device = malloc(sizeof(Device));
    stream->device = device;
    stream->device_instance = di;

    open_file_index = ((int) di) - BUTTON_DEVICE_ID_START;
    /* close the button if it is already open */
    /* i.e. it can only be open once */
    if (open_button_files[open_file_index] != NULL_STREAM) {
        fclose_button(open_button_files[open_file_index]);
    }
    open_button_files[open_file_index] = stream;

    return stream;
}

int fclose_button(Stream *stream) {
    int open_file_index;
    open_file_index = ((int) stream->device_instance) - BUTTON_DEVICE_ID_START;
    open_button_files[open_file_index] = NULL_STREAM;
    free(stream->device);
    free(stream);
    return 0;
}

int fgetc_button(Stream *stream) {
    if (stream->device_instance == BUTTON_SW1) {
        return sw1In();
    }
    if (stream->device_instance == BUTTON_SW2) {
        return sw2In();
    }
    return -1;
}

int fputc_button(int c) {
    return c;
}
