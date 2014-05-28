#ifndef IO_TOUCH_PAD_H
#define IO_TOUCH_PAD_H

void initialize_io_touch_pad(void);
int fputc_touch_pad(int c, Stream *stream);
int fgetc_touch_pad(Stream *stream);

#endif
