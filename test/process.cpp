#include "gtest/gtest.h"
#include "../third-party/fff.h"
DEFINE_FFF_GLOBALS;


#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>

extern "C" {
#include "../util/util.h"
#include "../memory/memory.h"
#include "../include/constants.h"
#include "../process/process.h"
}

/*
FAKE_VALUE_FUNC(uint64_t, svc_get_current_millis);

uint64_t svc_get_current_millis_value_fake(void) {
    struct timeval tv;
    uint64_t rv;
    gettimeofday(&tv, NULL);
    rv = 1000 * tv.tv_sec;
    rv += tv.tv_usec / 1000;
    return rv;
}
*/

class ProcessTest : public ::testing::Test {
    protected:

  // You can remove any or all of the following functions if its body
  // is empty.

  ProcessTest() {
    // You can do set-up work for each test here.
    initialize_memory();
  }

  virtual ~ProcessTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
    //RESET_FAKE(svc_get_current_millis);

    initialize_PCB_LIST();
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
    destroy_PCB_LIST();
  }
};

TEST_F(ProcessTest, CreateProcess) {
    uint16_t pid = create_process();
    EXPECT_LE(0, pid);
}

TEST_F(ProcessTest, CreatePCB) {
/*
     In order to schedule processes, your operating
     system should maintain a process list with a process control
     block (PCB) for each process. The process list should be
     organized as (one or more) circular linked lists of PCB's.

     Each process should be identified by a process id (PID) number.
     The PID number is returned when a new process is created and is
     passed to all supervisor calls that act on processes (e.g. to
     wait for a process, to kill a process).

     Each PCB should contain sufficient information to
          save and restore the state of the process,
          to identify the process (the PID),
          to indicate the state of the process (running, ready, blocked),
          to track the CPU time attributed to that process,
          to maintain information required by the operating system on a per-process basis (such as logical to physical device assignments).
*/

    uint16_t reference_id = next_process_id();
    struct PCB *iter;
    struct PCB *p, *q;

    p = create_pcb();
    EXPECT_EQ(reference_id + 1, p->PID);
    EXPECT_EQ(0, p->cpu_time);
    EXPECT_EQ(BLOCKED, p->state);
    EXPECT_EQ(PROCESS_QUANTUM, p->remaining_quantum);

    EXPECT_EQ((get_PCB_LIST())->next, p);

    q = create_pcb();
    EXPECT_EQ(reference_id + 2, q->PID);
    EXPECT_EQ(q->next, p);
    EXPECT_EQ(p->next, get_PCB_LIST());


    iter = get_PCB_LIST();
    EXPECT_GT(reference_id, iter->PID);

    iter = iter->next;
    EXPECT_EQ(reference_id + 2, iter->PID);

    iter = iter->next;
    EXPECT_EQ(reference_id + 1, iter->PID);

    iter = iter->next;
    EXPECT_EQ(iter, get_PCB_LIST());
}

TEST_F(ProcessTest, FindPCB) {
    struct PCB *p, *q;
    p = create_pcb();
    q = find_pcb(p->PID);
    EXPECT_EQ(p, q);
    q = find_pcb(999);
    EXPECT_EQ(NULL, q);
}

TEST_F(ProcessTest, DeletePCB) {
    struct PCB *p = create_pcb();
    uint16_t pid = p->PID;
    EXPECT_EQ(SUCCESS, delete_pcb(pid));

    EXPECT_EQ(CANNOT_DELETE_INIT_PROCESS, delete_pcb((get_PCB_LIST())->PID));
    EXPECT_EQ(PID_NOT_FOUND, delete_pcb(999));
}

TEST_F(ProcessTest, RunPauseProcess) {
    struct PCB *p;

    p = create_pcb();
    EXPECT_NE(p, get_current_process());

    //svc_get_current_millis_fake.custom_fake = svc_get_current_millis_value_fake;
    uint64_t pre_start_millis = svc_get_current_millis();
    run_process(p);
    uint64_t post_start_millis = svc_get_current_millis();

    EXPECT_EQ(p, get_current_process());
    EXPECT_EQ(RUNNING, p->state);
    EXPECT_LE(pre_start_millis, p->start_time_millis);
    EXPECT_GE(post_start_millis, p->start_time_millis);

    uint64_t pre_end_millis = svc_get_current_millis();
    end_process(p);
    uint64_t post_end_millis = svc_get_current_millis();

    EXPECT_EQ(COMPLETE, p->state);
    EXPECT_LE(pre_end_millis, p->end_time_millis);
    EXPECT_GE(post_end_millis, p->end_time_millis);

    uint64_t cpu_time_lower_bound = pre_end_millis - post_start_millis;
    uint64_t cpu_time_upper_bound = post_end_millis - pre_start_millis;
    EXPECT_LE(cpu_time_lower_bound, p->total_time_millis);
    EXPECT_GE(cpu_time_upper_bound, p->total_time_millis);
}

TEST_F(ProcessTest, ChooseProcessToRun) {
    struct PCB *p, *q, *r, *chosen;
    p = create_pcb();
    q = create_pcb();
    r = create_pcb();
    p->state = r->state = BLOCKED;
    q->state = READY;

    chosen = choose_process_to_run();
    EXPECT_EQ(q, chosen);
    run_process(chosen);

    p->state = READY;
    chosen = choose_process_to_run();
    EXPECT_EQ(p, chosen);
    run_process(chosen);

    r->state = READY;
    chosen = choose_process_to_run();
    EXPECT_EQ(r, chosen);
}

TEST_F(ProcessTest, DISABLED_TraverseProcessMemory) {

}

TEST_F(ProcessTest, DISABLED_FreeProcessMemory) {

}

TEST_F(ProcessTest, HandleQuantumExpired) {
    struct PCB *p, *q, *cp;
    p = create_pcb();
    q = create_pcb(); /* a second process to switch to */

    p->state = READY;
    q->state = READY;

    cp = choose_process_to_run();

    run_process(cp);

    EXPECT_EQ(cp, get_current_process());

    handle_quantum_expired(cp);
    EXPECT_NE(RUNNING, cp->state);
    EXPECT_NE(cp, get_current_process());
}

TEST_F(ProcessTest, QuantumInterrupt) {
    struct PCB *pcb = get_current_process();
    struct PCB *q;
    q = create_pcb(); /* a second process to switch to */

    uint32_t pre = pcb->remaining_quantum;
    quantum_interrupt();
    EXPECT_EQ(pre - 1, pcb->remaining_quantum);
}

TEST_F(ProcessTest, CreateStack) {
    struct ProcessStack *ps;
    ps = create_stack();
    EXPECT_EQ(STACK_SIZE, ps->size);
    EXPECT_NE((void *) NULL, ps->stack_pointer);
}

TEST_F(ProcessTest, FirstRun) {
    /* reset systick every time you schedule */
    EXPECT_EQ(1, 0);
}

TEST_F(ProcessTest, KillMeFlag) {
    /* checks for the kill-me flag on the next cycle and it will get rid of the doomed processes storage */
    EXPECT_EQ(1, 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

