#ifndef IO_H
#define IO_H

#include "io_constants.h"

void initialize_io(void);
int myCreate(const char *filename);
int myDelete(const char *filename);
Stream *myFopen(const char *filename);
int myFclose(Stream *stream);
int myFgetc(Stream *stream);
int myFputc(int c, Stream *stream);
Stream *find_stream(enum device_instance di);
int stream_is_led(Stream *stream);
int device_is_led(enum device_instance di);
int device_is_button(enum device_instance di);

#endif
