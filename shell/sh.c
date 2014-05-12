#if !defined IO_DEMO && !defined SVC_DEMO && !defined UART_DEMO \
&& !defined LCD_DEMO && !defined POTENTIOMETER_DEMO && !defined TOUCH_PAD_DEMO

#include "../init/init.h"
#include "shell.h"

int main(void) {
    initialize_system();
    run_shell();
    return 0;
}

#endif
