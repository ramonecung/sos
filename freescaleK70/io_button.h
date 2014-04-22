#ifndef IO_BUTTON_H
#define IO_BUTTON_H

#include "io.h"

#define NUMBER_BUTTONS 2

void initialize_io_button(void);

Stream *find_stream_button(enum device_instance di);

Stream *fopen_button(enum device_instance di);
int fclose_button(Stream *stream);

int fgetc_button(Stream *stream);
int fputc_button(int c);

#endif
