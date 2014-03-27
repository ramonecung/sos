#ifndef IO_H
#define IO_H

/* unneeded? */
enum device_type {
    LED
};

enum device_instance {
    LED_ORANGE,
    BUTTON_SW1
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

#endif
