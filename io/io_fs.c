#include "../include/constants.h"
#include "../include/io.h"
#include "io_fs.h"
#include "../util/strings.h"
#include "../memory/memory.h"
#include "../arm/critical_section.h"

static NamedFile file_list_head;
static NamedFile *FILE_LIST_HEAD;

void disable_interrupts_io_fs(void) {
    disable_interrupts();
}

void enable_interrupts_io_fs(void) {
    enable_interrupts();
}

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

    f = (NamedFile *) myMalloc(sizeof(NamedFile));
    if (f == NULL) {
        return CANNOT_CREATE_FILE;
    }

    /* set filename */
    filename_length = string_length(filename);
    f->filename = (const char *) myMalloc(filename_length + 1);
    if (f->filename == NULL) {
        myFree(f);
        return CANNOT_CREATE_FILE;
    }
    string_copy(filename, f->filename);


    /* create data block */
    f->first_block = create_block();
    if (f->first_block == NULL) {
        myFree((void *) f->filename);
        myFree(f);
        return CANNOT_CREATE_FILE;
    }
    f->last_block = f->first_block;
    f->size = 0;

    /* link into file list */
    disable_interrupts_io_fs();
    f->next = FILE_LIST_HEAD->next;
    FILE_LIST_HEAD->next = f;
    enable_interrupts_io_fs();

    /* we do not want the file's data to be freed */
    /* when the creating process dies */
    /* associate its memory with the init process */
    set_pid_for_pointer((void *) f, 0);
    set_pid_for_pointer((void *) f->filename, 0);

    return SUCCESS;
}

NamedFile *find_file(const char *filename) {
    NamedFile *cursor;
    disable_interrupts_io_fs();
    cursor = FILE_LIST_HEAD;
    while (cursor != NULL) {
        if (strings_equal(filename, (char *) cursor->filename)) {
            enable_interrupts_io_fs();
            return cursor;
        }
        cursor = cursor->next;
    }
    enable_interrupts_io_fs();
    return NULL;
}

int file_exists(const char *filename) {
    return (find_file(filename) != NULL);
}

int delete_fs(const char *filename) {
    NamedFile *cursor, *previous;
    disable_interrupts_io_fs();
    previous = cursor = FILE_LIST_HEAD;
    while (cursor != NULL) {
        if (strings_equal(filename, (char *) cursor->filename)) {
            previous->next = cursor->next; /* drop from linked list */
            free_file_blocks(cursor);
            myFree((void *) cursor->filename);
            myFree((void *) cursor);
            enable_interrupts_io_fs();
            return SUCCESS;
        } else {
            previous = cursor;
            cursor = cursor->next;
        }
    }
    enable_interrupts_io_fs();
    return CANNOT_DELETE_FILE;
}

void free_file_blocks(NamedFile *file) {
    Block *b1, *b2;
    b1 = file->first_block;
    while (b1 != NULL) {
        b2 = b1->next;
        myFree(b1);
        b1 = b2;
    }
}

int setup_stream_fs(Stream *stream, NamedFile *file) {
    stream->file = file;
    stream->read_block = file->first_block;
    stream->write_block = file->first_block;
    stream->next_byte_to_read = file->first_block->data;
    stream->next_byte_to_write = file->first_block->data;
    return SUCCESS;
}

int fputc_fs(int c, Stream *stream) {
    int stream_offset;
    int rv;
    disable_interrupts_io_fs();
    stream_offset = stream->next_byte_to_write - stream->write_block->data;
    if (stream_offset < BLOCK_SIZE) {
        rv = append_char(c, stream);
        enable_interrupts_io_fs();
        return rv;
    } else {
        if (advance_file_block(stream) != SUCCESS) {
            enable_interrupts_io_fs();
            return CANNOT_PUT_CHAR;
        }
        enable_interrupts_io_fs();
        return fputc_fs(c, stream); /* recursive call */
    }
}

int append_char(int c, Stream *stream) {
    int stream_offset;
    int eof_offset;
    stream_offset = stream->next_byte_to_write - stream->write_block->data;
    /* if appending to end of file update sizes */
    if (stream->write_block == stream->file->last_block) {
        eof_offset = stream->file->size % BLOCK_SIZE;
        if (stream_offset >= eof_offset) {
            stream->file->size++;
            stream->write_block->size++;
        }
    }
    *(stream->next_byte_to_write++) = c;
    return c;
}

int advance_file_block(Stream *stream) {
    Block *next_block;
    /* append a new block if needed */
    if (stream->write_block == stream->file->last_block) {
        next_block = create_block();
        if (next_block == NULL) {
            return CANNOT_APPEND_BLOCK;
        }
        stream->write_block->next = next_block;
        stream->file->last_block = next_block;
    }
    /* advance the block either way */
    stream->write_block = stream->write_block->next;
    stream->next_byte_to_write = stream->write_block->data;
    return SUCCESS;
}

int fgetc_fs(Stream *stream) {
    int read_so_far;
    int rv;
    disable_interrupts_io_fs();
    read_so_far = stream->next_byte_to_read - stream->read_block->data;
    if (read_so_far < stream->read_block->size) {
        rv = *(stream->next_byte_to_read++);
        enable_interrupts_io_fs();
        return rv;
    }
    if (read_so_far >= BLOCK_SIZE && stream->read_block->next != NULL) {
        stream->read_block = stream->read_block->next;
        stream->next_byte_to_read = stream->read_block->data;
        enable_interrupts_io_fs();
        return fgetc_fs(stream); /* recursive call */
    }
    enable_interrupts_io_fs();
    return EOF;
}


Block *create_block(void) {
    Block *new_block;
    new_block = myMalloc(sizeof(Block));
    if (new_block == NULL) {
        return NULL;
    }

    new_block->data = myMalloc(BLOCK_SIZE);
    if (new_block->data == NULL) {
        myFree((void *) new_block);
        return NULL;
    }
    new_block->size = 0;
    new_block->next = NULL;

    /* we do not want the file's data to be freed */
    /* when the creating process dies */
    /* associate its memory with the init process */
    set_pid_for_pointer((void *) new_block, 0);
    set_pid_for_pointer((void *) new_block->data, 0);

    return new_block;
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
