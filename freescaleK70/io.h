#ifndef IO_H
#define IO_H

#define MAX_OPEN_FILES 1024


#define NULL_STREAM ((Stream *) 0)

/* unneeded? */
enum device_type {
    LED
};

enum device_instance {
    FILE_SYSTEM = (MAX_OPEN_FILES),
    LED_ORANGE = (MAX_OPEN_FILES + 1),
    LED_YELLOW = (MAX_OPEN_FILES + 2),
    LED_GREEN = (MAX_OPEN_FILES + 3),
    LED_BLUE = (MAX_OPEN_FILES + 4),
    BUTTON_SW1 = (MAX_OPEN_FILES + 5),
    BUTTON_SW2 = (MAX_OPEN_FILES + 6)
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
