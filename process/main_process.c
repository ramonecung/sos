#ifdef PROCESS_DEMO

#include "../init/init.h"
#include "../process/process.h"
#include "../include/constants.h"
#include "../include/svc.h"

#include "../shell/ser2lcd.h"
#include "../shell/settimer.h"
#include <stdio.h>


static Stream *orange_led;
static int orange_led_on;
static uint32_t flash_orange_led_pid = -1;
static int interval = 23437; /* half a second */

void turn_on_led_and_reset_timer(void) {
    orange_led_on = orange_led_on ? 0 : 1;
    /* in handler mode, don't use svc call */
    myFputc(orange_led_on, orange_led);
    wake(flash_orange_led_pid);
}

int flash_orange_led(int argc, char **argv) {
    orange_led = svc_myFopen("/dev/led/orange");
    while(TRUE) {
        if (flash_orange_led_pid < 0) {
            continue;
        }
        svc_setTimer(interval, turn_on_led_and_reset_timer);
        svc_block();
    }
}

int sw2message(int argc, char **argv) {
    Stream *sw2;
    int c;
    uint32_t pid;
    char msg[64];
    pid = getpid();
    sw2 = svc_myFopen("/dev/button/sw2");
    while(TRUE) {
        c = svc_myFgetc(sw2);
        if (c == EOF) {
            svc_myFputs("sw2message: end of file received from button\r\n", STDOUT);
        }
        if (c) {
            sprintf(msg, "Hello from PID %d. sw2 was pressed\r\n", (int) pid);
            svc_myFputs(msg, STDOUT);
        }
    }
}

int openFiles(int argc, char **argv) {
    uint32_t pid;
    char msg[128];
    pid = getpid();
    sprintf(msg, "Hello from PID %d. Leaving 3 files open, will be closed on kill.\r\n", (int) pid);
    svc_myFputs(msg, STDOUT);
    svc_myFflush(STDOUT);
    svc_myFopen("/dev/led/blue");
    svc_myFopen("/dev/button/sw1");
    svc_myCreate("dev/fs/data");
    svc_myFopen("/dev/fs/data");
    sprintf(msg, "About to kill PID %d.\r\n", (int) pid);
    svc_myFputs(msg, STDOUT);
    svc_myFflush(STDOUT);
    /* kill me */
    svc_myKill(pid);
    return 0;
}

int main(void) {
    uint32_t pid;
    char msg[64];

    initialize_system();

    flash_orange_led_pid = svc_spawn(flash_orange_led);
    sprintf(msg, "spawned PID: %d\r\n", (int) flash_orange_led_pid);
    svc_myFputs(msg, STDOUT);

    pid = svc_spawn(openFiles);
    sprintf(msg, "spawned PID: %d\r\n", (int) pid);
    svc_myFputs(msg, STDOUT);

    pid = svc_spawn(cmd_ser2lcd);
    sprintf(msg, "spawned PID: %d\r\n", (int) pid);
    svc_myFputs(msg, STDOUT);

    pid = svc_spawn(sw2message);
    sprintf(msg, "spawned PID: %d\r\n", (int) pid);
    svc_myFputs(msg, STDOUT);

    init_process();

    return 0;
}
#endif
