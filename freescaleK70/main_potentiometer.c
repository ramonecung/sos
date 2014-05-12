#ifdef POTENTIOMETER_DEMO
/**
 * main.c
 * ADC demonstration project main program
 *
 * Demonstrates using the K70 analog to digital conversion hardware
 *
 * ARM-based K70F120M microcontroller board
 *   for educational purposes only
 * CSCI E-92 Spring 2014, Professor James L. Frankel, Harvard Extension School
 *
 * Written by James L. Frankel (frankel@seas.harvard.edu)
 */


#include <stdio.h>
#include <derivative.h>
#include "../include/constants.h"
#include "../util/util.h"
#include "../init/init.h"
#include "io.h"
#include "hardware/delay.h"
#include "hardware/svc.h"

#define POTENTIOMETER_ZERO 48 /* values bottom out at 45 - 48 */

int main(void) {
  Stream *s1, *s2;
  unsigned int c;

  initialize_system();
  s1 = svc_myFopen("/dev/adc/potentiometer");
  s2 = svc_myFopen("/dev/adc/thermistor");

  while(TRUE) {
        char formatted_output[64];
        c = (unsigned) svc_myFgetc(s1);
        sprintf(formatted_output, "pot: %4u\ttemp: %4u\r\n",
                c,
                svc_myFgetc(s2));
        delay(10000000);
        efputs(formatted_output, STDOUT);
        if (c <= POTENTIOMETER_ZERO) {
            efputs("Reached potentiometer min, exiting\r\n", STDOUT);
            break;
        }
  }

  svc_myFclose(s1);
  svc_myFclose(s2);
  return 0;
}
#endif
