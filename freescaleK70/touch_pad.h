#ifndef TOUCH_PAD_H
#define TOUCH_PAD_H

#define ELECTRODE_NUMBER_E1 0
#define ELECTRODE_NUMBER_E2 1
#define ELECTRODE_NUMBER_E3 2
#define ELECTRODE_NUMBER_E4 3

void TSI_Init(void);
void TSI_Calibrate(void);
int electrode_in(int electrodeNumber);

#endif
