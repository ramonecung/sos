#include "../include/constants.h"
#include "./RotateLED/pushbutton.h"
#include <stdlib.h>

#include "io.h"
#include "io_button.h"

Stream *fopen_button(enum device_instance di) {
    Stream *s = malloc(sizeof(Stream));
    s->device_instance = di;
    return s;
}
