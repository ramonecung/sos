#ifndef ONE_SHOT_TIMER_H
#define ONE_SHOT_TIMER_H

#include <stdint.h>

void setTimer(uint16_t count, void (*function)(void));

#endif
