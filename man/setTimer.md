# setTimer

\#include "timer/one\_shot\_timer.h"

This library provides the function:
- void svc\_setTimer(uint16\_t count, void (*function)(void));

The count parameter represents the number of 1 / 46785 second periods
to wait before executing the timerAction function. Pass in 46875 to
execute timerAction after 1 second.

The function parameter is a pointer the the function
that should execute when the time fires.

This function should be quite simple since it is called from the
interrupt routine and may be running on the interrupt stack and
with elevated interrupt priority level.

Note that this function could call svc\_setTimer itself to implement a
repeating interval timer.
