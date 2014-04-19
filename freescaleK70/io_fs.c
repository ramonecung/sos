#include "../include/constants.h"
#include "io.h"
#include "io_fs.h"
#include "../util/strings.h"
#include <stdlib.h>

static Stream *open_files[MAX_OPEN_FILES];

void initialize_io_fs(void) {
    unsigned short i;
    for (i = 0; i < MAX_OPEN_FILES; i++) {
        open_files[i] = NULL_STREAM;
    }
}

unsigned short next_file_id(void) {
    /* TODO: lock data! */
    unsigned short i;
    for (i = 0; i < MAX_OPEN_FILES; i++) {
        if (open_files[i] == NULL_STREAM) {
            return i;
        }
    }
    return i; /* invalid file id */
}

void purge_open_files(void) {
    unsigned short i;
    for (i = 0; i < MAX_OPEN_FILES; i++) {
        if (open_files[i] != NULL_STREAM) {
            fclose_fs(open_files[i]);
        }
        open_files[i] = NULL_STREAM;
    }
}

Stream *fopen_fs(const char *filename) {
    Stream *stream;
    Device *device;
    unsigned int file_id;
    stream = malloc(sizeof(Stream));
    device = malloc(sizeof(Device));
    stream->device = device;
    stream->device_instance = FILE_SYSTEM;
    stream->filename = (const char *) malloc(string_length(filename));
    string_copy(filename, stream->filename);
    file_id = next_file_id();
    if (file_id >= MAX_OPEN_FILES) {
        /* error */
        return NULL_STREAM;
    }
    stream->file_id = file_id;
    open_files[file_id] = stream;
    /* give every file 2 Kb */
    /* TODO: make this smarter */
    char *data = (char *) malloc(FILE_SIZE);
    if (data == NULL) {
        /* error */
        return NULL_STREAM;
    }
    stream->data = data;
    stream->last_byte = data;
    stream->next_byte_to_read = data;
    return stream;
}

int fclose_fs(Stream *stream) {
    open_files[stream->file_id] = NULL_STREAM;
    free(stream->device);
    free(stream);
    return 0;
}

int fputc_fs(int c, Stream *stream) {
    *(stream->last_byte++) = c;
    return c;
}

int fgetc_fs(Stream *stream) {
    return *(stream->next_byte_to_read++);
}

int filename_valid(const char *filename) {
    const char *basename;
    if (!prefix_valid(filename)) {
        return 0;
    }
    basename = filename + 8;
    if (!basename_valid(basename)) {
        return 0;
    }
    return 1;
}

int prefix_valid(const char *filename) {
    char prefix[] = "/dev/fs/"; /* 8 characters */
    int i;
    for (i = 0; i < 8; i++) {
        if (!filename[i]) {
            return 0;
        }
        if (filename[i] != prefix[i]) {
            return 0;
        }
    }
    return 1;
}

int basename_valid(const char *basename) {
    /*
     * restrict basename to POSIX Fully portable filenames according to
     * http://en.wikipedia.org/wiki/Filename#Comparison_of_filename_limitations
     * which means:
     * max length 14
     * A–Z a–z 0–9 . _ -
     * hyphen must not be first character
     */
    char *cp;
    int count;

    cp = (char *) basename;
    if (!*cp) {
        return 0;
    }
    if (*cp == '-') {
        return 0;
    }
    count = 0;
    while (*cp) {
        if (!valid_basename_character(*cp++)) {
            return 0;
        }
        if (++count > 14) {
            return 0;
        }
    }
    return 1;
}

int valid_basename_character(char c) {
    if (c >= 'A' && c <= 'Z') {
        return 1;
    }
    if (c >= 'a' && c <= 'z') {
        return 1;
    }
    if (c >= '0' && c <= '9') {
        return 1;
    }
    if (c == '.' || c == '_' || c == '-') {
        return 1;
    }
    return 0;
}