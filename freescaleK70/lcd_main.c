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
 * Written by James L. Frankel (frankel@seas.harvard.edu)
 */

/*
 * In addition to this file (main.c), this project requires the following files:
 *   lcdc.c
 *   lcdc.h
 *   lcdcConsole.c
 *   lcdcConsole.h
 *   mcg.c
 *   mcg.h
 *   profont.c
 *   profont.h
 *   sdram.c
 *   sdram.h
 *   uart.c
 *   uart.h
 */

#include "hardware/mcg.h"
#include "hardware/sdram.h"
#include "hardware/uart.h"
#include "hardware/lcdc.h"
#include "hardware/lcdcConsole.h"
#include "../init/init.h"

#define CHAR_EOF 4

void consoleDemo();

struct console console;
struct console *CONSOLE = &console;
void initialize_lcd(void) {
    lcdcInit();
    lcdcConsoleInit(CONSOLE);
}

int main(void) {
  initialize_system();

  initialize_lcd();

  consoleDemo(CONSOLE);

  return 0;
}

void consoleDemo(struct console *console) {
  while(1) {
  char ch = uartGetchar(UART2_BASE_PTR);

  // Echo the input character back to the UART
  uartPutchar(UART2_BASE_PTR, ch);

  // Output the character on the TWR_LCD_RGB
  lcdcConsolePutc(console, ch);

  // Exit if character typed was a Control-D (EOF)
  if(ch == CHAR_EOF) {
    return;
  }
  }
}
#endif
