#ifdef IO_DEMO

#include "../include/constants.h"
#include "../init/init.h"
#include "../util/util.h"
#include "io_constants.h"
#include "io.h"
#include "hardware/delay.h"
#include "../memory/memory.h"
#include "hardware/svc.h"

void consoleDemo(void) {
  Stream *s1, *s2;
  char ch;
  efputs("LCD Demo starting.\n", STDOUT);
  efputs("Type (via UART) to see characters on the console.\r\n", STDOUT);
  efputs("Type Control-D to terminate LCD demo.\r\n", STDOUT);
  s1 = svc_myFopen("/dev/uart/uart2");
  s2 = svc_myFopen("/dev/lcd/lcd1");
  while(1) {
    ch = svc_myFgetc(s1);

  // Echo the input character back to the UART
    svc_myFputc(ch, s1);

  // Output the character on the TWR_LCD_RGB
    svc_myFputc(ch, s2);

  // Exit if character typed was a Control-D (EOF)
    if(ch == CHAR_EOF) {
      efputs("LCD demo complete.\r\n", STDOUT);
      return;
    }
  }
}

void uart_demo(void) {
    char c;
    Stream *s;

    efputs("SerialIO Project Starting\r\n", STDOUT);
    efputs("Opening UART and waiting for character to be entered...\r\n", STDOUT);
    s = svc_myFopen("/dev/uart/uart2");
    c = svc_myFgetc(s);
    efputs("Got character, printing to UART...\r\n", STDOUT);
    svc_myFputc(c, s);
    efputs("\r\n", STDOUT);


    efputs("SerialIO Project Completed\r\n", STDOUT);
}

void fputc_led_demo(char *led) {
    Stream *stream;
    stream = svc_myFopen(led);

    efputs("Turn on ", STDOUT);
    efputs(led, STDOUT);
    efputs("\r\n", STDOUT)
    ;
    svc_myFputc(1, stream);
    delay(10000000);

    efputs("Turn off ", STDOUT);
    efputs(led, STDOUT);
    efputs("\r\n", STDOUT);

    svc_myFputc(0, stream);
    svc_myFclose(stream);
}

void fgetc_button_demo(void) {
    efputs("A total of 6 button presses will terminate program\r\n", STDOUT);
    Stream *stream1 = svc_myFopen("/dev/button/sw1");
    Stream *stream2 = svc_myFopen("/dev/button/sw2");
    int counter = 0;
    int sw1, sw2;
    while(1) {
        sw1 = svc_myFgetc(stream1);
        if (sw1) {
            counter++;
            efputs("SW1 pressed\r\n", STDOUT);
        }
        sw2 = svc_myFgetc(stream2);
        if (sw2) {
            counter++;
            efputs("SW2 pressed\r\n", STDOUT);
        }
        if (counter >= 6) {
            svc_myFclose(stream1);
            svc_myFclose(stream2);
            break;
        }
    }
}

void fs_demo(void) {
    Stream *stream;
    int a, b, c;

    efputs("Create /dev/fs/text...\r\n", STDOUT);
    svc_myCreate("/dev/fs/text");

    efputs("Open a stream on /dev/fs/text...\r\n", STDOUT);
    stream = svc_myFopen("/dev/fs/text");

    efputs("Write characters a, b, c to the stream...\r\n", STDOUT);
    svc_myFputc('a', stream);
    svc_myFputc('b', stream);
    svc_myFputc('c', stream);

    efputs("Read a character and print it\r\n", STDOUT);
    a = svc_myFgetc(stream);
    efputc(a, STDOUT);
    efputs("\r\n", STDOUT);

    efputs("Read a character and print it\r\n", STDOUT);
    b = svc_myFgetc(stream);
    efputc(b, STDOUT);
    efputs("\r\n", STDOUT);

    efputs("Read a character and print it\r\n", STDOUT);
    c = svc_myFgetc(stream);
    efputc(c, STDOUT);
    efputs("\r\n", STDOUT);

    efputs("Close the stream\r\n", STDOUT);
    svc_myFclose(stream);

    efputs("Delete /dev/fs/text\r\n", STDOUT);
    svc_myDelete("/dev/fs/text");
}

int main(void) {
    initialize_system();

    uart_demo();

    consoleDemo();

    efputs("In memory file-system:\r\n", STDOUT);
    fs_demo();

    efputs("LEDs:\r\n", STDOUT);
    fputc_led_demo("/dev/led/orange");
    fputc_led_demo("/dev/led/yellow");
    fputc_led_demo("/dev/led/green");
    fputc_led_demo("/dev/led/blue");

    efputs("Buttons:\r\n", STDOUT);
    fgetc_button_demo();

    efputs("Goodbye\r\n", STDOUT);
    return 0;
}

#endif
