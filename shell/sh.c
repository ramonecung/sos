#if !defined BUTTON_FLASH_DEMO && !defined IO_DEMO && !defined SVC_DEMO && \
!defined UART_DEMO && !defined LCD_DEMO && !defined POTENTIOMETER_DEMO && \
!defined TOUCH_PAD_DEMO && !defined FLEX_TIMER_DEMO && !defined SERIAL_INTERRUPT_DEMO

#include "../init/init.h"
#include "shell.h"

int main(void) {
    initialize_system();
    run_shell();
    return 0;
}

#endif
