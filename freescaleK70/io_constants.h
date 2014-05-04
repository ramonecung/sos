#ifndef IO_CONSTANTS_H
#define IO_CONSTANTS_H

#define BLOCK_SIZE 512

#ifndef EOF
#define EOF -1
#endif

/* unneeded? */
enum device_type {
    LED
};

enum device_instance {
    LED_ORANGE,
    LED_YELLOW,
    LED_GREEN,
    LED_BLUE,
    BUTTON_SW1,
    BUTTON_SW2,
    FILE_SYSTEM
};


struct Block {
    int size; /* num chars in this block */
    char *data;
    struct Block *next;
};
typedef struct Block Block;

struct NamedFile {
    const char *filename;
    int size; /* total number of bytes in file */
    Block *first_block; /* this file's data */
    struct NamedFile *next;
};
typedef struct NamedFile NamedFile;

struct Device {
    enum device_type type; // unneeded?
};
typedef struct Device Device;

struct Stream {
    enum device_instance device_instance;
    unsigned int stream_id;
    NamedFile *file;
    Block *current_block; /* use to know pointer to next block and current block size */
    char *next_byte_to_write;
    char *next_byte_to_read;
    struct Stream *next;
};
typedef struct Stream Stream;

#endif
