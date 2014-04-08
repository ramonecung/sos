#ifndef IO_BUTTON_H
#define IO_BUTTON_H

#include "io.h"

void initialize_io_button(void);

Stream *fopen_button(enum device_instance di);
int fclose_button(Stream *stream);

int fgetc_button(void);
int fputc_button(int c, Stream *stream);

void turn_on_button(enum device_instance di);
void turn_off_button(enum device_instance di);

#endif
