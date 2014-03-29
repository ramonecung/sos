#ifndef IO_LED_H
#define IO_LED_H

#include "io.h"

int fgetc_led(Stream *stream);
int fputc_led(int c, Stream *stream);

void turn_on_led(enum device_instance di);
void turn_off_led(enum device_instance di);

#endif
