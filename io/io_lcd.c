#include "../include/constants.h"
#include "../util/util.h"
#include "../init/init.h"
#include "../freescaleK70/lcdc.h"
#include "../freescaleK70/lcdcConsole.h"

#include "../include/io.h"
#include "io_lcd.h"

static struct console console;
static struct console *CONSOLE = &console;

void initialize_io_lcd(void) {
      lcdcInit();
      lcdcConsoleInit(CONSOLE);
}

int fputc_lcd(int c, Stream *stream) {
    if (stream->device_instance == LCD) {
        lcdcConsolePutc(CONSOLE, c);
        return c;
    } else {
        return CANNOT_PUT_CHAR;
    }
}

int fgetc_lcd(Stream *stream) {
    if (stream->device_instance == LCD) {
        return EOF;
    }
    return CANNOT_GET_CHAR;
}
