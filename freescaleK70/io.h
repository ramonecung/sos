#ifndef IO_H
#define IO_H

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
    BUTTON_SW2
};

struct Device {
    enum device_type type; // unneeded?
};
typedef struct Device Device;

struct Stream {
    Device *device;
    enum device_instance device_instance;
};
typedef struct Stream Stream;


Stream *myFopen(const char *filename);
int myFclose(Stream *stream);
int myFgetc(Stream *stream);

#endif
