#if !defined BUTTON_FLASH_DEMO && !defined IO_DEMO && !defined SVC_DEMO && \
!defined UART_DEMO && !defined LCD_DEMO && !defined POTENTIOMETER_DEMO && \
!defined TOUCH_PAD_DEMO && !defined FLEX_TIMER_DEMO && \
!defined SERIAL_INTERRUPT_DEMO && \
!defined ONESHOT_TIMER_DEMO && \
!defined PROCESS_DEMO

#include "../init/init.h"
#include "../include/svc.h"
#include "../include/constants.h"
#include "shell.h"

#include <stdio.h>

int main(void) {
    initialize_system();
    svc_spawn(run_shell);
    while(TRUE) {
        ;
    }
    return 0;
}

#endif
