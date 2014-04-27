#include "../include/constants.h"
#include "../freescaleK70/io.h"
#include "../util/util.h"
#include "../util/strings.h"
#include "../memory/memory.h"
#include "shell-io.h"

#ifdef TEST_SHELL
#include <stdio.h>
#endif

#define MAX_OUTPUT_STRING_LENGTH 64
static char output_string[MAX_OUTPUT_STRING_LENGTH];

/* private function declarations */
int is_valid_stream_id(int stream_id, char *arg);
Stream *find_stream_from_arg(char *arg);

/* public functions */
#ifdef TEST_SHELL
int cmd_create(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_create(int argc, char *argv[]) {
#endif
    int res;
    if (argc != 2) {
        return WRONG_NUMBER_ARGS;
    }
    res = myCreate(argv[1]);
    if (res != SUCCESS) {
        efputs("create: error creating file\n", ostrm);
    }
    return res;
}

#ifdef TEST_SHELL
int cmd_delete(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_delete(int argc, char *argv[]) {
#endif
    int res;
    if (argc != 2) {
        return WRONG_NUMBER_ARGS;
    }
    res = myDelete(argv[1]);
    if (res != SUCCESS) {
        efputs("delete: cannot delete file\n", ostrm);
    }
    return res;
}

#ifdef TEST_SHELL
int cmd_fopen(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_fopen(int argc, char *argv[]) {
#endif
    Stream *stream;
    if (argc != 2) {
        return WRONG_NUMBER_ARGS;
    }
    stream = myFopen(argv[1]);
    if (stream == NULL_STREAM) {
        efputs("fopen: error opening file\n", ostrm);
        return CANNOT_OPEN_FILE;
    }
    sprintf(output_string, "file opened with stream ID: %d\n", stream->device_instance);
    efputs(output_string, ostrm);
    return SUCCESS;
}

#ifdef TEST_SHELL
int cmd_fclose(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_fclose(int argc, char *argv[]) {
#endif
    Stream *stream;
    if (argc != 2) {
        return WRONG_NUMBER_ARGS;
    }
    stream = find_stream_from_arg(argv[1]);
    if (stream != NULL_STREAM) {
        return myFclose(stream);
    }
    efputs("fclose: cannot close stream\n", ostrm);
    return CANNOT_CLOSE_FILE;
}

#ifdef TEST_SHELL
int cmd_fgetc(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_fgetc(int argc, char *argv[]) {
#endif
    Stream *stream;
    int c, res;
    if (argc != 2) {
        return WRONG_NUMBER_ARGS;
    }
    stream = find_stream_from_arg(argv[1]);
    if (stream == NULL_STREAM) {
        efputs("fgetc: cannot get char from null stream\n", ostrm);
        return CANNOT_GET_CHAR;
    }
    c = myFgetc(stream);
    if (stream_is_button(stream) || stream_is_led(stream)) {
        /* shift for printing */
        c = c + '0';
    }
    /* print result to console */
    res = efputc(c, ostrm);
    res = efputc('\n', ostrm);
    if (res != SUCCESS) {
        efputs("fgetc: error printing char\n", ostrm);
        return res;
    }
    return SUCCESS;

}

#ifdef TEST_SHELL
int cmd_fputc(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_fputc(int argc, char *argv[]) {
#endif
    Stream *stream;
    int c, res;
    if (argc != 3) {
        efputs("usage: fputc streamID char\n", ostrm);
        return WRONG_NUMBER_ARGS;
    }
    if (string_length(argv[2]) > 1) {
        efputs("fputc: invalid char\n", ostrm);
        return INVALID_INPUT;
    }
    stream = find_stream_from_arg(argv[1]);
    if (stream != NULL_STREAM) {
        c = argv[2][0];
        /* shift to let caller turn off LED */
        if (stream_is_led(stream) && c == '0') {
            c = 0;
        }
        myFputc(c, stream);
        return SUCCESS;
    }
    efputs("fputc: cannot put char\n", ostrm);
    return CANNOT_PUT_CHAR;
}

/* private functions */
Stream *find_stream_from_arg(char *arg) {
    int stream_id = myAtoi(arg);
    if (!is_valid_stream_id(stream_id, arg)) {
        return NULL_STREAM;
    }
    return find_stream((enum device_instance) stream_id);
}

int is_valid_stream_id(int stream_id, char *arg) {
    /* TODO make this check more robust */
    if (stream_id <= 0 && arg[0] != '0') {
        return FALSE;
    }
    if (stream_id > MAX_DEVICE_INSTANCE) {
        return FALSE;
    }
    return TRUE;
}
