#include "../include/constants.h"
#include "io.h"
#include "io_fs.h"
#include "../util/strings.h"
#include "../util/util.h"
#include <stdlib.h>

static NamedFile file_list_head;
static NamedFile *FILE_LIST_HEAD;

void initialize_io_fs(void) {
    file_list_head.filename = "";
    file_list_head.first_block = NULL;
    file_list_head.next = NULL;
    FILE_LIST_HEAD = &file_list_head;
}

int create_fs(const char *filename) {
    NamedFile *f;
    int filename_length;

    if (!filename_valid(filename)) {
        return CANNOT_CREATE_FILE;
    }

    f = (NamedFile *) emalloc(sizeof(NamedFile), "create_fs", stderr);
    if (f == NULL) {
        return CANNOT_CREATE_FILE;
    }

    filename_length = string_length(filename);
    f->filename = (const char *) emalloc(filename_length + 1, "create_fs", stderr);
    if (f->filename == NULL) {
        efree(f);
        return CANNOT_CREATE_FILE;
    }
    string_copy(filename, f->filename);

    f->first_block = emalloc(sizeof(Block), "create_fs", stderr);
    if (f->first_block == NULL) {
        efree((void *) f->filename);
        efree(f);
        return CANNOT_CREATE_FILE;
    }

    f->first_block->data = emalloc(BLOCK_SIZE, "create_fs", stderr);
    if (f->first_block->data == NULL) {
        efree((void *) f->first_block);
        efree((void *) f->filename);
        efree(f);
        return CANNOT_CREATE_FILE;
    }

    f->size = 0;

    f->next = FILE_LIST_HEAD->next;
    FILE_LIST_HEAD->next = f;

    return SUCCESS;
}

NamedFile *find_file(const char *filename) {
    NamedFile *cursor;
    cursor = FILE_LIST_HEAD;
    while (cursor != NULL) {
        if (strings_equal(filename, (char *) cursor->filename)) {
            return cursor;
        }
        cursor = cursor->next;
    }
    return NULL;
}

int file_exists(const char *filename) {
    return (find_file(filename) != NULL);
}

int delete_fs(const char *filename) {
    NamedFile *cursor, *previous;
    previous = cursor = FILE_LIST_HEAD;
    while (cursor != NULL) {
        if (strings_equal(filename, (char *) cursor->filename)) {
            previous->next = cursor->next; /* drop from linked list */
            free_file_blocks(cursor);
            efree((void *) cursor->filename);
            efree((void *) cursor);
            return SUCCESS;
        } else {
            previous = cursor;
            cursor = cursor->next;
        }
    }
    return CANNOT_DELETE_FILE;
}

void free_file_blocks(NamedFile *file) {
    Block *b1, *b2;
    b1 = file->first_block;
    while (b1 != NULL) {
        b2 = b1->next;
        efree(b1);
        b1 = b2;
    }
}

int setup_stream_fs(Stream *stream, NamedFile *file) {
    stream->file = file;
    stream->next_byte_to_read = file->first_block->data;
    stream->next_byte_to_write = file->first_block->data;
    return SUCCESS;
}

int fputc_fs(int c, Stream *stream) {
    *(stream->next_byte_to_write++) = c;
    stream->file->size++;
    return c;
}

int fgetc_fs(Stream *stream) {
    if (stream->next_byte_to_read == stream->next_byte_to_write) {
        return EOF;
    }
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
