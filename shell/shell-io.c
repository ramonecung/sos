#include "../include/constants.h"
#include "../freescaleK70/io.h"
#include "../freescaleK70/hardware/svc.h"
#include "../util/util.h"
#include "../util/strings.h"
#include "../memory/memory.h"
#include "shell-io.h"

/* need for sprintf */
#include <stdio.h>

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
    res = svc_myCreate(argv[1]);
    if (res != SUCCESS) {
        efputs("create: error creating file\r\n", ostrm);
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
    res = svc_myDelete(argv[1]);
    if (res != SUCCESS) {
        efputs("delete: cannot delete file\r\n", ostrm);
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
    stream = svc_myFopen(argv[1]);
    if (stream == NULL) {
        efputs("fopen: error opening file\r\n", ostrm);
        return CANNOT_OPEN_FILE;
    }
    sprintf(output_string, "file opened with stream ID: %d\r\n", stream->stream_id);
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
    if (stream != NULL) {
        return svc_myFclose(stream);
    }
    efputs("fclose: cannot close stream\r\n", ostrm);
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
    if (stream == NULL) {
        efputs("fgetc: cannot get char from null stream\r\n", ostrm);
        return CANNOT_GET_CHAR;
    }
    c = svc_myFgetc(stream);
    if (stream_is_button(stream)) {
        /* shift for printing */
        c = c + '0';
    }
    if (c == EOF) {
        efputs("fgetc: end of file\r\n", ostrm);
        return SUCCESS;
    }
    /* print result to console */
    res = efputc(c, ostrm);
    res = efputs("\r\n", ostrm);
    if (res != SUCCESS) {
        efputs("fgetc: error printing char\r\n", ostrm);
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
    int c;
    if (argc != 3) {
        efputs("usage: fputc streamID char\r\n", ostrm);
        return WRONG_NUMBER_ARGS;
    }
    if (string_length(argv[2]) > 1) {
        efputs("fputc: invalid char\r\n", ostrm);
        return INVALID_INPUT;
    }
    stream = find_stream_from_arg(argv[1]);
    if (stream != NULL) {
        c = argv[2][0];
        /* shift to let caller turn off LED */
        if (stream_is_led(stream) && c == '0') {
            c = 0;
        }
        svc_myFputc(c, stream);
        return SUCCESS;
    }
    efputs("fputc: cannot put char\r\n", ostrm);
    return CANNOT_PUT_CHAR;
}

/* private functions */
Stream *find_stream_from_arg(char *arg) {
    int stream_id = myAtoi(arg);
    if (!is_valid_stream_id(stream_id, arg)) {
        return NULL;
    }
    return find_stream(stream_id);
}

int is_valid_stream_id(int stream_id, char *arg) {
    /* TODO make this check more robust */
    if (stream_id <= 0 && arg[0] != '0') {
        return FALSE;
    }
    return TRUE;
}
