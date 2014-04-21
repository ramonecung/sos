#include "../include/constants.h"
#include "io.h"
#include "io_fs.h"
#include "../util/strings.h"
#include "../util/util.h"
#include <stdlib.h>

static Stream *open_fs_files[MAX_OPEN_FILE_SYSTEM_FILES];
static NamedFile file_list_head;
static NamedFile *FILE_LIST_HEAD;
static NamedFile null_file;
static NamedFile *NULL_FILE;

void initialize_io_fs(void) {
    char *eof;
    unsigned short i;
    for (i = 0; i < MAX_OPEN_FILE_SYSTEM_FILES; i++) {
        open_fs_files[i] = NULL_STREAM;
    }

    eof = (char *) emalloc(1, "create_fs", stderr);
    eof[0] = 4;

    null_file.filename = "";
    null_file.data = eof;
    NULL_FILE = &null_file;

    file_list_head.filename = "";
    file_list_head.data = eof;
    file_list_head.next = NULL_FILE;
    FILE_LIST_HEAD = &file_list_head;
}

int create_fs(const char *filename) {
    NamedFile *cursor, *previous;
    NamedFile *f;
    char *data;
    int filename_length;

    /* create file data structure */
    if (!filename_valid(filename)) {
        return CANNOT_CREATE_FILE;
    }
    f = (NamedFile *) emalloc(sizeof(NamedFile), "create_fs", stderr);
    filename_length = string_length(filename);
    f->filename = (const char *) emalloc(filename_length, "create_fs", stderr);
    string_copy(filename, f->filename);

    /* give every file FILE_SIZE bytes */
    /* TODO: make this smarter */
    data = (char *) emalloc(FILE_SIZE, "create_fs", stderr);
    if (data == NULL) {
        /* error */
        return CANNOT_CREATE_FILE;
    }
    f->data = data;

    /* insert file into file list */
    f->next = NULL_FILE;
    previous = cursor = FILE_LIST_HEAD;
    while (cursor != NULL_FILE) {
        previous = cursor;
        cursor = cursor->next;
    }
    previous->next = f;

    return SUCCESS;
}

NamedFile *find_file(const char *filename) {
    NamedFile *cursor;
    cursor = FILE_LIST_HEAD;
    while (cursor != NULL_FILE) {
        if (strings_equal(filename, (char *) cursor->filename)) {
            return cursor;
        }
        cursor = cursor->next;
    }
    return NULL_FILE;
}

int file_exists(const char *filename) {
    return (find_file(filename) != NULL_FILE);
}

int delete_fs(const char *filename) {
    NamedFile *cursor, *previous;
    previous = cursor = FILE_LIST_HEAD;
    while (cursor != NULL_FILE) {
        if (strings_equal(filename, (char *) cursor->filename)) {
            /* drop from linked list */
            previous->next = cursor->next;
            free((void *) cursor->filename);
            free((void *) cursor->data);
            free((void *) cursor);
            return SUCCESS;
        } else {
            previous = cursor;
            cursor = cursor->next;
        }
    }
    return CANNOT_DELETE_FILE;
}

unsigned short next_file_id(void) {
    /* TODO: lock data! */
    unsigned short i;
    for (i = 0; i < MAX_OPEN_FILE_SYSTEM_FILES; i++) {
        if (open_fs_files[i] == NULL_STREAM) {
            return i;
        }
    }
    return i; /* invalid file id */
}

void purge_open_files_fs(void) {
    unsigned short i;
    for (i = 0; i < MAX_OPEN_FILE_SYSTEM_FILES; i++) {
        if (open_fs_files[i] != NULL_STREAM) {
            fclose_fs(open_fs_files[i]);
        }
        open_fs_files[i] = NULL_STREAM;
    }
}

Stream *fopen_fs(const char *filename) {
    NamedFile *file;
    Stream *stream;
    Device *device;
    unsigned int file_id;
    file = find_file(filename);
    if (file == NULL_FILE) {
        return NULL_STREAM;
    }
    stream = emalloc(sizeof(Stream), "fopen_fs", stderr);
    device = emalloc(sizeof(Device), "fopen_fs", stderr);
    stream->device = device;

    file_id = next_file_id();
    if (file_id >= MAX_OPEN_FILE_SYSTEM_FILES) {
        /* error */
        return NULL_STREAM;
    }
    stream->device_instance = (enum device_instance) file_id;
    open_fs_files[file_id] = stream;

    stream->next_byte_to_read = file->data;
    stream->last_byte = file->data;

    return stream;
}

int fclose_fs(Stream *stream) {
    open_fs_files[stream->device_instance] = NULL_STREAM;
    free((void *) stream->device);
    free((void *) stream);
    return SUCCESS;
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
        return FALSE;
    }
    basename = filename + 8;
    if (!basename_valid(basename)) {
        return FALSE;
    }
    return TRUE;
}

int prefix_valid(const char *filename) {
    char prefix[] = "/dev/fs/"; /* 8 characters */
    int i;
    for (i = 0; i < 8; i++) {
        if (!filename[i]) {
            return FALSE;
        }
        if (filename[i] != prefix[i]) {
            return FALSE;
        }
    }
    return TRUE;
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
        return FALSE;
    }
    if (*cp == '-') {
        return FALSE;
    }
    count = 0;
    while (*cp) {
        if (!valid_basename_character(*cp++)) {
            return FALSE;
        }
        if (++count > 14) {
            return FALSE;
        }
    }
    return TRUE;
}

int valid_basename_character(char c) {
    if (c >= 'A' && c <= 'Z') {
        return TRUE;
    }
    if (c >= 'a' && c <= 'z') {
        return TRUE;
    }
    if (c >= '0' && c <= '9') {
        return TRUE;
    }
    if (c == '.' || c == '_' || c == '-') {
        return TRUE;
    }
    return FALSE;
}
