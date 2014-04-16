#ifndef IO_H
#define IO_H

#define NULL_STREAM ((Stream *) 0)

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

struct Device {
    enum device_type type; // unneeded?
};
typedef struct Device Device;

struct Stream {
    Device *device;
    enum device_instance device_instance;
    unsigned int file_id;
};
typedef struct Stream Stream;

void initialize_io(void);
Stream *myFopen(const char *filename);
int myFclose(Stream *stream);
int myFgetc(Stream *stream);
int myFputc(int c, Stream *stream);

#endif
