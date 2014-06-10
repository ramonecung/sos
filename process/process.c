
#include <stdint.h>

#include <sys/time.h>

#include "../util/util.h"
#include "../memory/memory.h"
#include "../include/constants.h"
#include "process.h"


uint64_t svc_get_current_millis(void) {
    struct timeval tv;
    uint64_t rv;
    gettimeofday(&tv, NULL);
    rv = 1000 * tv.tv_sec;
    rv += tv.tv_usec / 1000;
    return rv;
}



    struct PCB *PCB_LIST;
    struct PCB *current_process;




    void initialize_process_manager(void) {
        initialize_PCB_LIST();
    }

    struct PCB *get_current_process(void) {
        return current_process;
    }

    struct PCB *get_PCB_LIST(void) {
        return PCB_LIST;
    }

    uint16_t create_process(void) {
        struct PCB *pcb = create_pcb();
        pcb->process_stack = create_stack();
        return pcb->PID;
    }

    struct ProcessStack *create_stack(void) {
        struct ProcessStack *ps =
            (struct ProcessStack *) myMalloc(sizeof(struct ProcessStack));
        if (ps == NULL) {
            return NULL;
        }
        ps->size = STACK_SIZE;
        ps->stack_pointer = myMalloc(sizeof(STACK_SIZE));
        if (ps->stack_pointer == NULL) {
            myFree(ps);
            return NULL;
        }
        return ps;
    }

    struct PCB *create_pcb(void) {
        struct PCB *pcb = (struct PCB *) myMalloc(sizeof(struct PCB));
        setup_pcb(pcb);
        insert_pcb(pcb);
        return pcb;
    }

    /* PCB and PCB list functions */

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
        pcb->cpu_time = 0;
        pcb->remaining_quantum = PROCESS_QUANTUM;
        pcb->process_stack = NULL;
    }


    void initialize_PCB_LIST(void) {
        struct PCB *init_process = (struct PCB *) myMalloc(sizeof(struct PCB));
        PCB_LIST = init_process;
        current_process = init_process;
        init_process->next = init_process;
        setup_pcb(init_process);
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

    void run_process(struct PCB *pcb) {
        if (pcb == NULL) {
            return;
        }
        current_process = pcb;
        pcb->state = RUNNING;
        pcb->start_time_millis = svc_get_current_millis();
    }

    /* do not call this if you haven't called run_process because start_time won't be set */
    void pause_process(struct PCB *pcb) {
        if (pcb == NULL) {
            return;
        }
        save_process_state(pcb);
        pcb->state = READY; /* this should vary */
    }

    void reclaim_storage(struct PCB *pcb) {
        if (pcb->state != COMPLETE && pcb->state != KILLED)
            return;
    }

    void end_process(struct PCB *pcb) {
        pcb->end_time_millis = svc_get_current_millis();
        pcb->total_time_millis = pcb->end_time_millis - pcb->start_time_millis;
        pcb->state = COMPLETE;
    }

    void save_process_state(struct PCB *pcb) {
        ;
        // save stack pointer
        // save registers. which?
        // push reg on stack?
        /*
         * In addition to pushing process A's registers then saving
         * process A's stack pointer, we also need to push a flag
         * indicating whether that process was interrupted
         * while an SVC handler was active.
         */
         //svcPushState();
    }

    void quantum_interrupt(void) {
        struct PCB *pcb = get_current_process();
        pcb->remaining_quantum--;
        if (pcb->remaining_quantum <= 0) {
            handle_quantum_expired(pcb);
        }
    }

    void handle_quantum_expired(struct PCB *pcb) {
        pause_process(pcb);
        run_process(choose_process_to_run());
    }
