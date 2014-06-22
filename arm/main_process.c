#ifdef PROCESS_DEMO

#include "../init/init.h"
#include "../process/process.h"
#include "../include/constants.h"
#include "../include/svc.h"

#include <stdio.h>

int dp2(void) {
    struct PCB *pcb;
    char msg[64];
    pcb = get_current_process();
    sprintf(msg, "hi from dp2 PID %d\r\n", pcb->PID);
    svc_myFputs(msg, STDOUT);
    svc_myFflush(STDOUT);
    svc_wake(3);
    svc_myKill(4);
    return 0;
}

int dp3(void) {
    struct PCB *pcb;
    char msg[64];
    pcb = get_current_process();
    sprintf(msg, "hi from dp3 PID %d\r\n", pcb->PID);
    svc_myFputs(msg, STDOUT);
    svc_block();
    sprintf(msg, "leaving dp3 now PID %d\r\n", pcb->PID);
    svc_myFputs(msg, STDOUT);
    svc_myFflush(STDOUT);
    return 0;
}

int dp4(void) {
    struct PCB *pcb;
    char msg[64];
    pcb = get_current_process();
    sprintf(msg, "hi from dp4 PID %d\r\n", pcb->PID);
    while(TRUE) {
        svc_myFputs(msg, STDOUT);
    }
    return 0;
}

int main(void) {
    struct PCB *p, *q, *r, *s;
    uint16_t pid;

    initialize_system();
    pid = svc_spawn();
    p = find_pcb(pid);
    p->state = READY;
    pid = svc_spawn();
    q = find_pcb(pid);
    *(q->initial_function) = (uint32_t) dp2;
    q->state = READY;
    pid = svc_spawn();
    r = find_pcb(pid);
    *(r->initial_function) = (uint32_t) dp3;
    r->state = READY;
    pid = svc_spawn();
    s = find_pcb(pid);
    *(s->initial_function) = (uint32_t) dp4;
    s->state = READY;

    init_process();

    return 0;
}
#endif
