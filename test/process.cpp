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
}

FAKE_VALUE_FUNC(uint64_t, svc_get_current_millis);

uint64_t svc_get_current_millis_value_fake(void) {
    struct timeval tv;
    uint64_t rv;
    gettimeofday(&tv, NULL);
    rv = 1000 * tv.tv_sec;
    rv += tv.tv_usec / 1000;
    return rv;
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
        uint64_t cpu_time;
        uint64_t start_time_millis;
        uint64_t end_time_millis;
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

    /* return SUCCESS if PCB with PID found and delete */
    /* PID_NOT_FOUND otherwise */
    int delete_pcb(uint16_t PID) {
        struct PCB *iter, *prev;
        iter = prev = process_list;
        if (PID == 0) {
            return CANNOT_DELETE_INIT_PROCESS;
        }
        do {
            if (iter->PID == PID) {
                prev->next = iter->next;
                myFree(iter);
                return SUCCESS;
            }
            prev = iter;
            iter = iter->next;
        } while (iter != process_list);
        return PID_NOT_FOUND;
    }

    void insert_pcb(struct PCB *pcb) {
        pcb->next = process_list->next;
        process_list->next = pcb;
    }


    uint16_t next_process_id(void) {
        static uint16_t process_id_sequence;
        return process_id_sequence++;
    }

    void setup_pcb(struct PCB *pcb) {

        pcb->state = BLOCKED;
        pcb->PID = next_process_id();
        pcb->cpu_time = 0;
        pcb->start_time_millis = pcb->end_time_millis = 0;
    }


    void run_process(uint16_t PID) {
        struct PCB *pcb = find_pcb(PID);
        if (pcb == NULL) {
            return;
        }
        pcb->state = RUNNING;
        pcb->start_time_millis = svc_get_current_millis();
    }

    /* do not call this if you haven't called run_process because start_time won't be set */
    void pause_process(uint16_t PID) {
        struct PCB *pcb = find_pcb(PID);
        if (pcb == NULL) {
            return;
        }
        pcb->state = READY; /* this should vary */
        pcb->end_time_millis = svc_get_current_millis();
        pcb->cpu_time += pcb->end_time_millis - pcb->start_time_millis;
    }

    void initialize_process_list(void) {
        struct PCB *init_process = (struct PCB *) myMalloc(sizeof(struct PCB));
        process_list = init_process;
        init_process->next = init_process;
        setup_pcb(init_process);
    }

    struct PCB *find_pcb(uint16_t PID) {
        struct PCB *iter = process_list;
        do {
            if (iter->PID == PID) {
                return iter;
            }
            iter = iter->next;
        } while (iter != process_list);
        return NULL;
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
    RESET_FAKE(svc_get_current_millis);

    initialize_process_list();
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
};

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
    EXPECT_EQ(p->start_time_millis, p->end_time_millis);
    EXPECT_EQ(BLOCKED, p->state);
    EXPECT_EQ(process_list->next, p);

    q = create_pcb();
    EXPECT_EQ(reference_id + 2, q->PID);
    EXPECT_EQ(q->next, p);
    EXPECT_EQ(p->next, process_list);


    iter = process_list;
    EXPECT_EQ(0, iter->PID);

    iter = iter->next;
    EXPECT_EQ(reference_id + 2, iter->PID);

    iter = iter->next;
    EXPECT_EQ(reference_id + 1, iter->PID);

    iter = iter->next;
    EXPECT_EQ(0, iter->PID);
    EXPECT_EQ(iter, process_list);
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

    EXPECT_EQ(CANNOT_DELETE_INIT_PROCESS, delete_pcb(0));
    EXPECT_EQ(PID_NOT_FOUND, delete_pcb(999));
}

TEST_F(ProcessTest, RunPauseProcess) {
    struct PCB *p;

    p = create_pcb();
    svc_get_current_millis_fake.custom_fake = svc_get_current_millis_value_fake;
    uint64_t pre_start_millis = svc_get_current_millis();
    run_process(p->PID);
    uint64_t post_start_millis = svc_get_current_millis();

    EXPECT_EQ(RUNNING, p->state);
    EXPECT_LE(pre_start_millis, p->start_time_millis);
    EXPECT_GE(post_start_millis, p->start_time_millis);

    uint64_t pre_end_millis = svc_get_current_millis();
    pause_process(p->PID);
    uint64_t post_end_millis = svc_get_current_millis();

    EXPECT_NE(RUNNING, p->state);
    EXPECT_LE(pre_end_millis, p->end_time_millis);
    EXPECT_GE(post_end_millis, p->end_time_millis);

    uint64_t cpu_time_lower_bound = pre_end_millis - post_start_millis;
    uint64_t cpu_time_upper_bound = post_end_millis - pre_start_millis;
    EXPECT_LE(cpu_time_lower_bound, p->cpu_time);
    EXPECT_GE(cpu_time_upper_bound, p->cpu_time);
}




int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
