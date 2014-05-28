#ifndef IO_LCD_H
#define IO_LCD_H

#include "../include/io.h"

void initialize_io_lcd(void);

int fgetc_lcd(Stream *stream);
int fputc_lcd(int c, Stream *stream);

#endif
