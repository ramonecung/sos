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

#include <stdint.h>
#include <stdio.h>
#include <derivative.h>
#include "io.h"
#include "../util/util.h"
#include "../init/init.h"

#define FALSE 0
#define TRUE 1

#define ADC_CHANNEL_POTENTIOMETER     0x14
#define ADC_CHANNEL_TEMPERATURE_SENSOR  0x1A

#define ADC_CFG1_MODE_8_9_BIT       0x0
#define ADC_CFG1_MODE_12_13_BIT     0x1
#define ADC_CFG1_MODE_10_11_BIT     0x2
#define ADC_CFG1_MODE_16_BIT        0x3
#define ADC_SC3_AVGS_32_SAMPLES     0x3

void adc_init(void) {
   SIM_SCGC3 |= SIM_SCGC3_ADC1_MASK;
   ADC1_CFG1 = ADC_CFG1_MODE(ADC_CFG1_MODE_12_13_BIT);
   ADC1_SC3 = ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(ADC_SC3_AVGS_32_SAMPLES);
}

unsigned int adc_read(uint8_t channel) {
   ADC1_SC1A = channel;
   while(!(ADC1_SC1A & ADC_SC1_COCO_MASK)) {
   }
   return ADC1_RA;
}

int main(void) {
   initialize_system();
   adc_init();

   while(TRUE) {
    char formatted_output[64];
    sprintf(formatted_output, "pot: %4u\ttemp: %4u\n",
           adc_read(ADC_CHANNEL_POTENTIOMETER),
           adc_read(ADC_CHANNEL_TEMPERATURE_SENSOR));
      efputs(formatted_output, STDOUT);
   }

   return 0;
}
#endif
