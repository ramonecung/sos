#ifndef IO_H
#define IO_H

#include "../io/io_constants.h"
#include "../process/process.h"

void initialize_io(void);

int myCreate(const char *filename);
int myDelete(const char *filename);
Stream *myFopen(const char *filename);
int myFclose(Stream *stream);
void close_all_streams_for_pid(unsigned int pid);
int myFgetc(Stream *stream);
char *myFgets(char *str, int size, Stream *stream);
int myFputc(int c, Stream *stream);
int myFputs(const char *s, Stream *stream);
int myFflush(Stream *stream);

Stream *create_stream(void);
void link_stream(Stream *stream);
void unlink_stream(Stream *stream);
Stream *find_stream(unsigned int stream_id);
unsigned int next_stream_id(void);
enum device_instance device_instance_from_filename(const char *filename);

int stream_is_touch_pad(Stream *stream);
int stream_is_potentiometer(Stream *stream);
int stream_is_thermistor(Stream *stream);
int stream_is_lcd(Stream *stream);
int stream_is_uart(Stream *stream);
int stream_is_led(Stream *stream);
int stream_is_button(Stream *stream);

int device_is_touch_pad(enum device_instance di);
int device_is_potentiometer(enum device_instance di);
int device_is_thermistor(enum device_instance di);
int device_is_lcd(enum device_instance di);
int device_is_uart(enum device_instance di);
int device_is_led(enum device_instance di);
int device_is_button(enum device_instance di);

extern Stream *standard_input;
extern Stream *standard_output;
extern Stream *standard_error;

#ifdef TEST_SHELL
#define STDIN stdin
#define STDOUT stdout
#define STDERR stderr
#else
#define STDIN (process_istrm())
#define STDOUT (process_ostrm())
#define STDERR (process_estrm())
#endif

#endif
