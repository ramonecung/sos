
#include <stdint.h>
#include <stdio.h>
#include <derivative.h>
#include "../include/svc.h"

#include "../util/util.h"
#include "../memory/memory.h"
#include "../include/constants.h"
#include "../include/io.h"
#include "../arm/critical_section.h"

#include "process.h"


int create_stack(struct PCB *pcb);
void preload_stack(struct PCB *pcb);

/* global variables used to manage process list */
struct PCB *PCB_LIST;
struct PCB *current_process;
uint32_t PROCESS_ID_SEQUENCE = 0;
int process_manager_initialized = FALSE;

void disable_interrupts_process(void) {
    disable_interrupts();
}

void enable_interrupts_process(void) {
    enable_interrupts();
}

/* only called during startup */
void initialize_process_manager(void) {
    if (!process_manager_initialized) {
        process_manager_initialized = TRUE;
        initialize_PCB_LIST();
        current_process->state = READY;
    }
}

void initialize_PCB_LIST(void) {
    struct PCB *init_pcb;
    init_pcb = create_process();
    PCB_LIST = init_pcb;
    *(init_pcb->initial_function) = (uint32_t) init_process;
    init_pcb->next = init_pcb;
    current_process = init_pcb;
}
/* end startup code */

void init_process(void) {
    while(TRUE) {
        svc_yield();
    }
}

/* public functions */
uint32_t getpid(void) {
    if (!process_manager_initialized) {
        return 0;
    }
    return current_process->PID;
}

uint32_t *stack_pointer_for_pid(uint32_t pid) {
    struct PCB *pcb = find_pcb(pid);
    if (pcb == NULL) {
        return 0;
    }
    return pcb->stack_pointer;
}

void save_stack_pointer_for_pid(uint32_t pid, uint32_t *sp) {
    struct PCB *pcb = find_pcb(pid);
    if (pcb != NULL) {
        pcb->stack_pointer = sp;
    }
}

uint32_t *stack_pointer_for_init_process(void) {
    return PCB_LIST->stack_pointer;
}

uint32_t next_pid_to_run(void) {
    struct PCB *pcb = choose_process_to_run();
    return pcb->PID;
}

void pause_process(uint32_t pid) {
    struct PCB *pcb = find_pcb(pid);
    if (pcb == NULL) {
        return;
    }
    pcb->total_cpu_time += PROCESS_QUANTUM;
    /* might have already updated state to KILLED or BLOCKED upstream */
    if (pcb->state == RUNNING) {
        pcb->state = READY;
    }
}

void schedule_process(uint32_t pid) {
    struct PCB *pcb = find_pcb(pid);
    if (pcb == NULL) {
        return;
    }
    current_process = pcb;
    pcb->state = RUNNING;
}

void yield(void) {
    /* OR in the PENDSVSET bit into the ICSR register */
    /* to provoke PendSV interrupt */
    SCB_ICSR |= SCB_ICSR_PENDSVSET_MASK;
}

void block(void) {
    struct PCB *pcb = get_current_process();
    pcb->state = BLOCKED;
    yield();
}

void wake(uint32_t pid) {
    struct PCB *pcb = find_pcb(pid);
    pcb->state = READY;
}

void myKill(uint32_t pid) {
    struct PCB *pcb;
    if (pid == 0) {
        return;
    }
    pcb = find_pcb(pid);
    if (pcb == NULL) {
        return;
    }
    pcb->end_time_millis = get_current_millis();
    pcb->total_time_millis = pcb->end_time_millis - pcb->start_time_millis;
    pcb->state = KILLED;
    myFclose(pcb->standard_input);
    myFclose(pcb->standard_output);
    myFclose(pcb->standard_error);
    if (pcb == get_current_process()) {
        yield();
    }
}

uint32_t spawn_process(int (*mainfunc)(void)) {
    struct PCB *pcb = create_process();
    insert_pcb(pcb);
    pcb->start_time_millis = get_current_millis();
    if (mainfunc != NULL) {
        *(pcb->initial_function) = (uint32_t) mainfunc;
    }
    pcb->state = READY;
    return pcb->PID;
}

void insert_pcb(struct PCB *pcb) {
    disable_interrupts_process();
    pcb->next = PCB_LIST->next;
    PCB_LIST->next = pcb;
    enable_interrupts_process();
}

struct PCB *create_process(void) {
    struct PCB *pcb = create_pcb();
    if (pcb == NULL) {
        return NULL;
    }
    if (create_stack(pcb) != SUCCESS) {
        myFree(pcb);
        return NULL;
    }
    initialize_standard_streams(pcb);
    return pcb;
}

void initialize_standard_streams(struct PCB *pcb) {
    /* create bi-directional io stream for stdin, stdout, stderr */
    pcb->standard_input = myFopen("/dev/uart/uart2");
    pcb->standard_output = myFopen("/dev/uart/uart2");
    pcb->standard_error = myFopen("/dev/uart/uart2");
}

Stream *process_istrm(void) {
    Stream *s;
    s = current_process->standard_input;
    return s;
}
Stream *process_ostrm(void) {
    Stream *s;
    s = current_process->standard_output;
    return s;
}

Stream *process_estrm(void) {
    Stream *s;
    s = current_process->standard_error;
    return s;
}

struct PCB *create_pcb(void) {
    struct PCB *pcb = (struct PCB *) myMalloc(sizeof(struct PCB));
    if (pcb == NULL) {
        return NULL;
    }
    initialize_pcb(pcb);
    return pcb;
}

void initialize_pcb(struct PCB *pcb) {
    pcb->state = BLOCKED;
    pcb->PID = next_process_id();
    pcb->total_cpu_time = 0;
}

uint32_t next_process_id(void) {
    uint32_t next_pid;
    disable_interrupts_process();
    next_pid = PROCESS_ID_SEQUENCE++;
    enable_interrupts_process();
    return next_pid;
}

int create_stack(struct PCB *pcb) {
    pcb->allocated_stack_address = myMalloc(STACK_SIZE);
    if (pcb->allocated_stack_address == NULL) {
        return CANNOT_ALLOCATE_STACK;
    }
    pcb->stack_size = STACK_SIZE;
    pcb->stack_pointer = pcb->allocated_stack_address + STACK_SIZE;
    preload_stack(pcb);
    return SUCCESS;
}

void preload_stack (struct PCB *pcb) {
    int dummy_process(void);
    void kill_me(void);
    *(--pcb->stack_pointer) = 0x01000000; /* XPSR set thumb bit */
    *(--pcb->stack_pointer) = (uint32_t) dummy_process; /* Return address - function pointer to start this process */
    pcb->initial_function = pcb->stack_pointer; /* use reference later to update RA to the real first instruction */
    *(--pcb->stack_pointer) = (uint32_t) kill_me; /* LR return to main stack, thread mode, basic frame */

    *(--pcb->stack_pointer) = 0; /* R12 */
    *(--pcb->stack_pointer) = 0; /* R3 */
    *(--pcb->stack_pointer) = 0; /* R2 */
    *(--pcb->stack_pointer) = 0; /* R1 - argv */
    *(--pcb->stack_pointer) = 0; /* R0 - argc */

    *(--pcb->stack_pointer) = 0xFFFFFFF9; /* LR that would be pushed for ISR, handler mode, main stack, basic frame */
    *(--pcb->stack_pointer) = 0; /* R7 - would be pushed by systick ISR (and set to stack pointer) */
    *(--pcb->stack_pointer) = 0; /* R4 - would be pushed by systick ISR */

    /* space for local variables created by systick ISR */
    *(--pcb->stack_pointer) = 0;
    *(--pcb->stack_pointer) = 0;
    *(--pcb->stack_pointer) = 0;
    *(--pcb->stack_pointer) = 0;
    *(--pcb->stack_pointer) = 0;

    *(--pcb->stack_pointer) = 0; /* R11 */
    *(--pcb->stack_pointer) = 0; /* R10 */
    *(--pcb->stack_pointer) = 0; /* R9 */
    *(--pcb->stack_pointer) = 0; /* R8 */
    *(--pcb->stack_pointer) = 0; /* R6 */
    *(--pcb->stack_pointer) = 0; /* R5 */

    *(--pcb->stack_pointer) = 0; /* SVCALLACT */
}

int dummy_process(void) {
    char msg[64];
    struct PCB *pcb = get_current_process();
    sprintf(msg, "hello from dummy, PID %d\r\n", pcb->PID);
    svc_myFputs(msg, STDOUT);
    svc_myFflush(STDOUT);
    return 0;
}

void kill_me(void) {
    struct PCB *pcb = get_current_process();
    svc_myKill(pcb->PID);
}

void reap(void) {
    disable_interrupts_process();
    /* this is somewhat inefficient since it traverses */
    /* all processes on every run */
    struct PCB *prev, *cur, *start;
    prev = start = PCB_LIST;
    cur = prev->next;
    /* cur will never be NULL since it's a circular queue */
    while(cur != start) {
        if (cur->state == KILLED) {
            prev->next = cur->next;
            reclaim_storage(cur);
            cur = prev->next;
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
    enable_interrupts_process();
}

void reclaim_storage(struct PCB *pcb) {
    myFreeAllForPID(pcb->PID);
    myFree(pcb->allocated_stack_address);
    myFree(pcb);
}

struct PCB *get_current_process(void) {
    return current_process;
}

struct PCB *find_pcb(uint32_t PID) {
    disable_interrupts_process();
    struct PCB *iter = PCB_LIST;
    do {
        if (iter->PID == PID) {
            enable_interrupts_process();
            return iter;
        }
        iter = iter->next;
    } while (iter != PCB_LIST);
    enable_interrupts_process();
    return NULL;
}

struct PCB *choose_process_to_run(void) {
    disable_interrupts_process();
    struct PCB *iter = get_current_process();
    while (TRUE) {
        iter = iter->next;
        if (iter->state == READY) {
            enable_interrupts_process();
            return iter;
        }
    }
}

