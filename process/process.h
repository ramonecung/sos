#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

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

    struct PCB *next;
};

/* public functions */
void initialize_process_manager(void);
uint16_t getpid(void);
uint16_t spawn_process(void);
void yield(void);
void block(void);
void wake(uint16_t pid);
void myKill(uint16_t pid);

uint32_t *stack_pointer_for_pid(uint16_t pid);
uint32_t *stack_pointer_for_init_process(void);

/* should not be public */
struct PCB *get_current_process(void);
void set_current_process(struct PCB *pcb);
struct PCB *create_process(void);
struct PCB *choose_process_to_run(void);
void init_process(void);

/* private function declarations */
/* exposed for testing */
void initialize_PCB_LIST(void);
struct PCB *get_PCB_LIST(void);
struct PCB *create_pcb(void);
void setup_pcb(struct PCB *pcb);
void insert_pcb(struct PCB *pcb);
int delete_pcb(uint16_t PID);
void save_process_state(struct PCB *pcb);
void handle_quantum_expired(struct PCB *pcb);
uint16_t next_process_id(void);
void destroy_processes_besides_init(void);

void destroy_PCB_LIST(void);
struct PCB *find_pcb(uint16_t PID);
void run_process(struct PCB *pcb);
void pause_process(struct PCB *pcb);
void reclaim_storage(struct PCB *pcb);
void end_process(struct PCB *pcb);
void save_process_state(struct PCB *pcb);
void quantum_interrupt(void);
void handle_quantum_expired(struct PCB *pcb);


uint64_t svc_get_current_millis(void);

#endif
