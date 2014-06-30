#include <stdio.h>
#include <stdint.h>

#include "../init/init.h"
#include "../process/process.h"
#include "../include/constants.h"
#include "../include/svc.h"
#include "../time/time.h"

#include "shell.h"
#include "ser2lcd.h"
#include "touch2led.h"
#include "pot2ser.h"
#include "settimer.h"

static Stream *orange_led;
static int orange_led_on;
static uint32_t flash_orange_led_pid = -1;
static int interval = 23437; /* half a second */
uint64_t start_millis, current_millis;
void turn_on_led_and_reset_timer(void) {
    char msg[64];
    orange_led_on = orange_led_on ? 0 : 1;
    /* in handler mode, don't use svc call */
    current_millis = get_current_millis();
    myFputc(orange_led_on, orange_led);
    if (current_millis - start_millis > 30000) {
        sprintf(msg, "About to kill PID %d.\r\n", (int) flash_orange_led_pid);
        myFputs(msg, STDOUT);
        myFputc(0, orange_led);
        myKill(flash_orange_led_pid);
    } else {
        wake(flash_orange_led_pid);
    }

}

int flash_orange_led(int argc, char **argv) {
    orange_led = svc_myFopen("/dev/led/orange");
    start_millis = svc_get_current_millis();
    while(TRUE) {
        if (flash_orange_led_pid < 0) {
            continue;
        }
        svc_setTimer(interval, turn_on_led_and_reset_timer);
        current_millis = svc_get_current_millis();
        svc_block();
    }
}

/* there is a race condition when reading the button value */
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
    Stream *lcd;

    pid = getpid();

    lcd = svc_myFopen("/dev/lcd/lcd");
    svc_myFopen("/dev/button/sw1");
    svc_myCreate("dev/fs/data");
    svc_myFopen("/dev/fs/data");

    sprintf(msg, "Hello from PID %d. Leaving 3 files open, will be closed on kill.\r\n", (int) pid);
    svc_myFputs(msg, STDOUT);
    svc_myFflush(STDOUT);
    svc_myFputs(msg, lcd);

    sprintf(msg, "About to kill PID %d.\r\n", (int) pid);
    svc_myFputs(msg, STDOUT);
    svc_myFflush(STDOUT);
    svc_myFputs(msg, lcd);
    /* kill me */
    svc_myKill(pid);
    return 0;
}

#ifdef TEST_SHELL
int cmd_process_demo(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_process_demo(int argc, char *argv[]) {
#endif
    uint32_t pid;
    char msg[64];

    flash_orange_led_pid = svc_spawn(flash_orange_led);
    sprintf(msg, "spawned PID: %d\r\n", (int) flash_orange_led_pid);
    svc_myFputs(msg, STDOUT);

    pid = svc_spawn(openFiles);
    sprintf(msg, "spawned PID: %d\r\n", (int) pid);
    svc_myFputs(msg, STDOUT);

    pid = svc_spawn(cmd_touch2led);
    sprintf(msg, "spawned PID: %d\r\n", (int) pid);
    svc_myFputs(msg, STDOUT);

    pid = svc_spawn(cmd_pot2ser);
    sprintf(msg, "spawned PID: %d\r\n", (int) pid);
    svc_myFputs(msg, STDOUT);

//    pid = svc_spawn(sw2message);
//    sprintf(msg, "spawned PID: %d\r\n", (int) pid);
//    svc_myFputs(msg, STDOUT);

    return 0;
}

#ifdef PROCESS_DEMO
int main(int argc, char **argv) {
    initialize_system();
    cmd_process_demo(0, NULL);
    init_process();
    return 0;
}
#endif
