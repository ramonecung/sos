#ifdef LCD_DEMO
/*
 * main.c
 * LCDRGB main program
 *
 * Main module to demonstrate input from UART and output to TWR-LCD-RGB
 *
 * ARM-based K70F120M microcontroller board
 *   for educational purposes only
 * CSCI E-92 Spring 2014, Professor James L. Frankel, Harvard Extension School
 *
 * Written by James L. Frankel (frankel@seas.harvard.edu) and Stephen Karger
 */

#include "../init/init.h"
#include "../include/io.h"
#include "../include/svc.h"

#define CHAR_EOF 4

void consoleDemo(void);


int main(void) {
  initialize_system();
  consoleDemo();
  return 0;
}

void consoleDemo(void) {
  Stream *s1, *s2;
  char ch;
  s1 = svc_myFopen("/dev/uart/uart2");
  s2 = svc_myFopen("/dev/lcd/lcd");
  while(1) {
    ch = svc_myFgetc(s1);

  // Echo the input character back to the UART
    svc_myFputc(ch, s1);

  // Output the character on the TWR_LCD_RGB
    svc_myFputc(ch, s2);

  // Exit if character typed was a Control-D (EOF)
    if(ch == CHAR_EOF) {
      return;
    }
  }
}
#endif
