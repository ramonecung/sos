#ifndef IO_H
#define IO_H

#define MAX_OPEN_FILE_SYSTEM_FILES 1024
#define LED_DEVICE_ID_START (MAX_OPEN_FILE_SYSTEM_FILES + 1)
#define BUTTON_DEVICE_ID_START (MAX_OPEN_FILE_SYSTEM_FILES + 5)


#define NULL_STREAM ((Stream *) 0)

/* unneeded? */
enum device_type {
    LED
};

enum device_instance {
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

void initialize_io(void);
int myCreate(const char *filename);
int myDelete(const char *filename);
Stream *myFopen(const char *filename);
int myFclose(Stream *stream);
int myFgetc(Stream *stream);
int myFputc(int c, Stream *stream);

#endif
