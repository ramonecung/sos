#ifndef TIME_H
#define TIME_H

#include <stdint.h>


void intFlexTimerInit(void);
void flexTimer0Action(void);

uint64_t current_millis(void);

#endif
