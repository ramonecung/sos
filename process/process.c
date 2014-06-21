
#include <stdint.h>
#include <stdio.h>
#include <derivative.h>
#include "../include/svc.h"

#include "../util/util.h"
#include "../memory/memory.h"
#include "../include/constants.h"
#include "../time/time.h"
#include "../include/svc.h"
#include "process.h"


uint64_t svc_get_current_millis(void) {
    struct timeval tv;
    uint64_t rv;
    svc_gettimeofday(&tv, NULL);
    rv = 1000 * tv.tv_sec;
    rv += tv.tv_usec / 1000;
    return rv;
}



int create_stack(struct PCB *pcb);
void preload_stack(struct PCB *pcb);

/* global variables used to manage process list */
struct PCB *PCB_LIST;
struct PCB *current_process;

void initialize_process_manager(void) {
	initialize_PCB_LIST();
	current_process->state = READY;
}

void initialize_PCB_LIST(void) {
	struct PCB *init_pcb;
	init_pcb = create_process();
	PCB_LIST = init_pcb;
	current_process = init_pcb;
	init_pcb->next = init_pcb;
}

struct PCB *get_PCB_LIST(void) {
	return PCB_LIST;
}


struct PCB *get_current_process(void) {
	return current_process;
}

uint16_t spawn_process(void) {
	struct PCB *pcb = create_process();
	insert_pcb(pcb);
	pcb->start_time_millis = get_current_millis();
	return pcb->PID;
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
	return pcb;
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
	int dummy_main(void);
	void kill_me(void);
	*(--pcb->stack_pointer) = 0x01000000; /* XPSR set thumb bit */
	*(--pcb->stack_pointer) = (uint32_t) dummy_main; /* Return address - function pointer to start this process */
	*(--pcb->stack_pointer) = (uint32_t) kill_me; /* LR return to main stack, thread mode, basic frame */
	/* Make LR "kill me" code, which will set pending kill on my PCB (so you can kill yourself without interrupts enabled). Go to current PCB and set bit. Then while(1) yield.
	 * if PID 0 do not kill
	 */
	*(--pcb->stack_pointer) = 0; /* R12 */
	*(--pcb->stack_pointer) = 0; /* R3 */
	*(--pcb->stack_pointer) = 0; /* R2 */
	*(--pcb->stack_pointer) = 0; /* R1 - argv */
	*(--pcb->stack_pointer) = 0; /* R0 - argc */

	*(--pcb->stack_pointer) = 0xFFFFFFF9; /* LR that would be pushed for ISR, handler mode, main stack, basic frame */
	*(--pcb->stack_pointer) = 0; /* R7 - will be set to stack pointer by ISR */

	/* space for local variables created by ISR */
	*(--pcb->stack_pointer) = 0;
	*(--pcb->stack_pointer) = 0;

	*(--pcb->stack_pointer) = 0; /* R11 */
	*(--pcb->stack_pointer) = 0; /* R10 */
	*(--pcb->stack_pointer) = 0; /* R9 */
	*(--pcb->stack_pointer) = 0; /* R8 */
	*(--pcb->stack_pointer) = 0; /* R6 */
	*(--pcb->stack_pointer) = 0; /* R5 */
	*(--pcb->stack_pointer) = 0; /* R4 */

	*(--pcb->stack_pointer) = 0; /* SVCALLACT */
}

int dummy_main(void) {
	char msg[64];
	struct PCB *pcb = get_current_process();
	sprintf(msg, "hello from PID %d\r\n", pcb->PID);
	svc_myFputs(msg, STDOUT);
	return 0;
}


void yield(void) {
	/* OR in the PENDSVSET bit into the ICSR register */
	/* to provoke PendSV interrupt */
	SCB_ICSR |= SCB_ICSR_PENDSVSET_MASK;
}

void kill_me(void) {
	struct PCB *pcb = get_current_process();
	pcb->end_time_millis = get_current_millis();
	pcb->total_time_millis = pcb->end_time_millis - pcb->start_time_millis;
	pcb->state = KILLED;
	svc_yield();
}

void reclaim_storage(struct PCB *pcb) {
	if (pcb->state != KILLED)
		return;
}


struct PCB *create_pcb(void) {
	struct PCB *pcb = (struct PCB *) myMalloc(sizeof(struct PCB));
	if (pcb == NULL) {
		return NULL;
	}
	setup_pcb(pcb);
	return pcb;
}

void insert_pcb(struct PCB *pcb) {
	pcb->next = PCB_LIST->next;
	PCB_LIST->next = pcb;
}

uint16_t next_process_id(void) {
	static uint16_t process_id_sequence;
	return process_id_sequence++;
}

void setup_pcb(struct PCB *pcb) {
	pcb->state = BLOCKED;
	pcb->PID = next_process_id();
	pcb->total_cpu_time = 0;
}

/* return SUCCESS if PCB with PID found and delete */
/* PID_NOT_FOUND otherwise */
int delete_pcb(uint16_t PID) {
	struct PCB *iter, *prev;
	iter = prev = PCB_LIST;
	do {
		if (iter->PID == PID) {
			if (iter == PCB_LIST) {
				return CANNOT_DELETE_INIT_PROCESS;
			}
			prev->next = iter->next;
			myFree(iter);
			return SUCCESS;
		}
		prev = iter;
		iter = iter->next;
	} while (iter != PCB_LIST);
	return PID_NOT_FOUND;
}

/* convenience methods for testing */
void destroy_processes_besides_init(void) {
	struct PCB *iter, *prev;
	iter = PCB_LIST->next;
	while (iter != PCB_LIST) {
		prev = iter;
		iter = iter->next;
		delete_pcb(prev->PID);
	}
}

void destroy_PCB_LIST(void) {
	destroy_processes_besides_init();
	delete_pcb(PCB_LIST->PID);
}

struct PCB *find_pcb(uint16_t PID) {
	struct PCB *iter = PCB_LIST;
	do {
		if (iter->PID == PID) {
			return iter;
		}
		iter = iter->next;
	} while (iter != PCB_LIST);
	return NULL;
}

/* schedule and run */
struct PCB *choose_process_to_run(void) {
	struct PCB *iter = get_current_process();
	while (TRUE) {
		iter = iter->next;
		if (iter->state == READY) {
			return iter;
		}
	}
}

