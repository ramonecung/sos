#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "../include/io.h"


#define STACK_SIZE 4096
#define PROCESS_QUANTUM 4800000 /* ticks per 40 ms at 120 MHz */

enum process_state {
    RUNNING,
    READY,
    BLOCKED,
    KILLED
};

struct PCB {
    int PID;
    enum process_state state;

    uint64_t total_cpu_time;

    /* for tracking wall clock time */
    uint64_t start_time_millis;
    uint64_t end_time_millis;
    uint64_t total_time_millis;

    /* stack */
    void *allocated_stack_address;
    uint32_t stack_size;
    uint32_t *stack_pointer;
    uint32_t *initial_function;

    /* io */
    Stream *standard_input;
    Stream *standard_output;
    Stream *standard_error;

    struct PCB *next;
};

/* public functions */
void initialize_process_manager(void);
uint32_t getpid(void);
uint32_t next_pid_to_run(void);
void schedule_process(uint32_t pid);
uint32_t spawn_process(int (*mainfunc)(void));
void yield(void);
void block(void);
void wake(uint32_t pid);
void myKill(uint32_t pid);

uint32_t *stack_pointer_for_init_process(void);
uint32_t *stack_pointer_for_pid(uint32_t pid);
void save_stack_pointer_for_pid(uint32_t pid, uint32_t *sp);

Stream *process_istrm(void);
Stream *process_ostrm(void);
Stream *process_estrm(void);


/* private function declarations */
/* exposed for testing */
void initialize_PCB_LIST(void);
struct PCB *create_pcb(void);
void initialize_pcb(struct PCB *pcb);
void insert_pcb(struct PCB *pcb);
uint32_t next_process_id(void);
void initialize_standard_streams(struct PCB *pcb);

void destroy_PCB_LIST(void);
struct PCB *find_pcb(uint32_t PID);
void run_process(struct PCB *pcb);
void pause_process(uint32_t pid);
struct PCB *get_current_process(void);
struct PCB *create_process(void);
struct PCB *choose_process_to_run(void);
void reap(void);
void reclaim_storage(struct PCB *pcb);

void init_process(void);

#endif
