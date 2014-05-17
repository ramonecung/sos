#include <stdint.h>
#include "hardware/flexTimer.h"

/*
 * Need to support years 1900 to 3000
 * Given:
 * 1100 years
 * 86,400,000 milliseconds in 1 day
 * Either 365 or 366 days in 1 year
 * Then
 * (86,400,000 * 365 * 1100) < num milliseconds in 1100 years < (86,400,000 * 366 * 1100)
 * 3.46896e+13 < num milliseconds in 1100 years < 3.478464e+13
 * and since
 * 2^44 < 3.46896e+13 < 3.478464e+13 < 2^45 - 1
 * we therefore need at least 45 bits (unsigned) to accommodate the number of milliseconds.
 */
uint64_t milliseconds_since_epoch;

/* set flextimer to interrupt every millisecond and increment */

/*
 * perform overall initialization
 */
void intFlexTimerInit(void) {
    /* set interval to 1 millisecond (1875 1/1875000 second periods) */
    flexTimer0Init(1875);
}

/*
 * this routine contains all actions to be performed when a FlexTimer 0
 * interrupt occurs.
 */
void flexTimer0Action(void) {
    milliseconds_since_epoch++;
}

uint64_t current_millis(void) {
    return milliseconds_since_epoch;
}
