#include "gtest/gtest.h"
#include <stdint.h>
#include <stdlib.h>
extern "C" {

//#include "../shell/shell.h"
#include "../util/util.h"
#include "../memory/memory.h"
}

class ProcessTest : public ::testing::Test {
    protected:

  // You can remove any or all of the following functions if its body
  // is empty.

    enum process_state {
        RUNNING,
        READY,
        BLOCKED
    };

    struct PCB {
        int PID;
        enum process_state state;
        uint32_t cpu_time;
        struct PCB *next;
    };
    struct PCB *process_list;

    uint16_t create_process(void) {
        struct PCB *pcb = create_pcb();
        return pcb->PID;
    }

    struct PCB *create_pcb(void) {
        struct PCB *pcb = (struct PCB *) myMalloc(sizeof(struct PCB));
        setup_pcb(pcb);
        insert_pcb(pcb);
        return pcb;
    }

    void insert_pcb(struct PCB *pcb) {
        pcb->next = process_list->next;
        process_list->next = pcb;
    }

    void setup_pcb(struct PCB *pcb) {
        static uint16_t process_id_sequence;
        pcb->state = BLOCKED;
        pcb->PID = process_id_sequence++;
        pcb->cpu_time = 0;
    }

    void initialize_process_list(void) {
        struct PCB *init_process = (struct PCB *) myMalloc(sizeof(struct PCB));
        process_list = init_process;
        init_process->next = init_process;
        setup_pcb(init_process);
    }

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
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
};

TEST_F(ProcessTest, X) {
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


    initialize_process_list();

    struct PCB *p = create_pcb();
    EXPECT_EQ(1, p->PID);

    EXPECT_EQ(process_list->next, p);

    struct PCB *q = create_pcb();
    EXPECT_EQ(2, q->PID);
    EXPECT_EQ(q->next, p);
    EXPECT_EQ(p->next, process_list);

    struct PCB *iter;
    iter = process_list;
    EXPECT_EQ(0, iter->PID);
    iter = iter->next;
    EXPECT_EQ(2, iter->PID);
    EXPECT_EQ(BLOCKED, iter->state);
    EXPECT_EQ(0, iter->cpu_time);
    iter = iter->next;
    EXPECT_EQ(1, iter->PID);
    iter = iter->next;
    EXPECT_EQ(0, iter->PID);
    EXPECT_EQ(iter, process_list);

}




int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
