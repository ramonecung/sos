
#ifndef _SYSTICK_H
#define _SYSTICK_H

#define Systick_MaxPriority 15
#define Systick_PriorityShift 4

/* Second lowest priority (14): quantum interrupt */
#define Systick_Priority 14

#define PROCESS_QUANTUM 4800000

void systickInit_SetSystickPriority(unsigned char priority);
void systickInit(void);
void systickIsr(void);

#endif
