# setTimer

\#include "timer/one\_shot\_timer.h"

This library provides the function:
- void svc\_setTimer(uint16\_t count);

The count parameter represents the number of 1 / 46785 second periods
to wait before executing the timerAction function. Pass in 46875 to
execute timerAction after 1 second.

The caller must define
void timerAction(void);

This function should be quite simple since it is called from the
interrupt routine and may be running on the interrupt stack and
with elevated interrupt priority level.
