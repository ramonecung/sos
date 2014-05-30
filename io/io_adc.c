#include "../include/io.h"
#include "../freescaleK70/adc.h"

void initialize_io_adc(void) {
	adc_init();
}

int fgetc_potentiometer(Stream *stream) {
	if (stream->device_instance == POTENTIOMETER) {
		return adc_read(ADC_CHANNEL_POTENTIOMETER);
	}
	return CANNOT_GET_CHAR;
}

int fputc_potentiometer(int c, Stream *stream) {
    return c;
}

int fgetc_thermistor(Stream *stream) {
    if (stream->device_instance == THERMISTOR) {
        return adc_read(ADC_CHANNEL_TEMPERATURE_SENSOR);
    }
    return CANNOT_GET_CHAR;
}

int fputc_thermistor(int c, Stream *stream) {
    return c;
}
