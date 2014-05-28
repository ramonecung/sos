#ifndef IO_UART_H
#define IO_UART_H

#include "io.h"

void initialize_io_uart(void);

int fgetc_uart(Stream *stream);
int fputc_uart(int c, Stream *stream);

void fflush_uart(void);

#endif
