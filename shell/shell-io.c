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
    return SUCCESS;
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
    enum device_instance di;
    int stream_id;
    if (argc != 2) {
        return WRONG_NUMBER_ARGS;
    }

    stream_id = myAtoi(argv[1]);
    /* TODO make this check more robust */
    if (stream_id <= 0 && argv[1][0] != '0') {
        efputs("fclose: invalid stream ID\n", ostrm);
        return CANNOT_CLOSE_FILE;
    }
    if (stream_id > MAX_DEVICE_INSTANCE) {
        efputs("fclose: invalid stream ID\n", ostrm);
        return CANNOT_CLOSE_FILE;
    }
    di = (enum device_instance) stream_id;
    stream = find_stream(di);
    return myFclose(stream);
}
