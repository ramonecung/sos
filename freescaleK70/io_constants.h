#ifndef IO_CONSTANTS_H
#define IO_CONSTANTS_H

#define MAX_OPEN_FILE_SYSTEM_FILES 32
#define FILE_SYSTEM_ID_START 0
#define FILE_SYSTEM_ID_END (MAX_OPEN_FILE_SYSTEM_FILES - 1)
#define LED_DEVICE_ID_START (MAX_OPEN_FILE_SYSTEM_FILES + 1)
#define BUTTON_DEVICE_ID_START (MAX_OPEN_FILE_SYSTEM_FILES + 5)
#define MAX_DEVICE_INSTANCE (MAX_OPEN_FILE_SYSTEM_FILES + 6)

#ifndef EOF
#define EOF -1 /* only use this as a return value from fgetc */
#endif

/* unneeded? */
enum device_type {
    LED
};

enum device_instance {
    NULL_DEVICE,
    LED_ORANGE = LED_DEVICE_ID_START,
    LED_YELLOW = LED_DEVICE_ID_START + 1,
    LED_GREEN = LED_DEVICE_ID_START + 2,
    LED_BLUE = LED_DEVICE_ID_START + 3,
    BUTTON_SW1 = BUTTON_DEVICE_ID_START,
    BUTTON_SW2 = BUTTON_DEVICE_ID_START + 1,
    FILE_SYSTEM
};

struct Device {
    enum device_type type; // unneeded?
};
typedef struct Device Device;

struct Stream {
    Device *device;
    enum device_instance device_instance;
    NamedFile *file;
    Block *current_block; /* use to know pointer to next block and current block size */
    char *last_byte;
    char *next_byte_to_read;
};
typedef struct Stream Stream;

extern Stream *NULL_STREAM;

#endif
