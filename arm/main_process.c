#ifdef PROCESS_DEMO

#include "../init/init.h"
#include "../process/process.h"
#include "../include/constants.h"
#include "../include/svc.h"

#include <stdio.h>

int dp2(void) {
    uint32_t pid;
    char msg[64];
    pid = getpid();
    sprintf(msg, "hi from dp2 PID %d\r\n", (int) pid);
    svc_myFputs(msg, STDOUT);
    svc_myFflush(STDOUT);
    svc_wake(3);
    svc_myKill(4);
    return 0;
}

int dp3(void) {
    uint32_t pid;
    char msg[64];
    pid = getpid();
    sprintf(msg, "hi from dp3 PID %d\r\n", (int) pid);
    svc_myFputs(msg, STDOUT);
    svc_block();
    sprintf(msg, "leaving dp3 now PID %d\r\n", (int) pid);
    svc_myFputs(msg, STDOUT);
    svc_myFflush(STDOUT);
    return 0;
}

int dp4(void) {
    uint32_t pid;
    char msg[64];
    pid = getpid();
    sprintf(msg, "hi from dp4 PID %d\r\n", (int) pid);
    while(TRUE) {
        svc_myFputs(msg, STDOUT);
    }
    return 0;
}

int main(void) {
    uint32_t pid;
    char msg[64];

    initialize_system();

    pid = svc_spawn(NULL);
    sprintf(msg, "spawned PID: %d\r\n", (int) pid);
    svc_myFputs(msg, STDOUT);

    pid = svc_spawn(dp2);
    sprintf(msg, "spawned PID: %d\r\n", (int) pid);
    svc_myFputs(msg, STDOUT);

    pid = svc_spawn(dp3);
    sprintf(msg, "spawned PID: %d\r\n", (int) pid);
    svc_myFputs(msg, STDOUT);

    pid = svc_spawn(dp4);
    sprintf(msg, "spawned PID: %d\r\n", (int) pid);
    svc_myFputs(msg, STDOUT);

    init_process();

    return 0;
}
#endif
