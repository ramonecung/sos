
#ifndef _SYSTICK_H
#define _SYSTICK_H

#define Systick_MaxPriority 15
#define Systick_PriorityShift 4

/* Second lowest priority (14): quantum interrupt */
#define Systick_Priority 14

void systickInit_SetSystickPriority(unsigned char priority);
void systickIsr(void);

#endif
