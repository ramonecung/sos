#include "../include/constants.h"
#include "../freescaleK70/io.h"
#include "../util/util.h"
#include "../util/strings.h"
#include "shell-io.h"

#ifdef TEST_SHELL
#include <stdio.h>
#endif

#define MAX_OUTPUT_STRING_LENGTH 64
static char output_string[MAX_OUTPUT_STRING_LENGTH];

extern Stream *NULL_STREAM;

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

