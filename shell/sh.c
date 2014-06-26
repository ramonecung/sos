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

int dp1(void) {
    uint32_t pid;
    char msg[64];
    void *vp;
    pid = getpid();
    vp = svc_myMalloc(256);
    sprintf(msg, "hi from dp1 PID %d\r\n", (int) pid);
    svc_myFputs(msg, STDOUT);
    svc_myFree(vp);
    return 0;
}

int main(void) {
    initialize_system();
    svc_spawn(run_shell);
    while(TRUE) {
        ;
    }
    return 0;
}

#endif
