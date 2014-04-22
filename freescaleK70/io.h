#ifndef IO_H
#define IO_H

#define MAX_OPEN_FILE_SYSTEM_FILES 1024
#define FILE_SYSTEM_ID_START 0
#define FILE_SYSTEM_ID_END (MAX_OPEN_FILE_SYSTEM_FILES - 1)
#define LED_DEVICE_ID_START (MAX_OPEN_FILE_SYSTEM_FILES + 1)
#define BUTTON_DEVICE_ID_START (MAX_OPEN_FILE_SYSTEM_FILES + 5)
#define MAX_DEVICE_INSTANCE (MAX_OPEN_FILE_SYSTEM_FILES + 6)

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
    BUTTON_SW2 = BUTTON_DEVICE_ID_START + 1
};

struct Device {
    enum device_type type; // unneeded?
};
typedef struct Device Device;

struct Stream {
    Device *device;
    enum device_instance device_instance;
    char *last_byte;
    char *next_byte_to_read;
};
typedef struct Stream Stream;

extern Stream *NULL_STREAM;

void initialize_io(void);
int myCreate(const char *filename);
int myDelete(const char *filename);
Stream *myFopen(const char *filename);
int myFclose(Stream *stream);
int myFgetc(Stream *stream);
int myFputc(int c, Stream *stream);
Stream *find_stream(enum device_instance di);

#endif
