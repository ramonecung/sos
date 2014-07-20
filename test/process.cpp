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


FAKE_VALUE_FUNC(uint64_t, get_current_millis);
FAKE_VALUE_FUNC(uint64_t, svc_get_current_millis);
uint64_t svc_get_current_millis_value_fake(void) {
    struct timeval tv;
    uint64_t rv;
    gettimeofday(&tv, NULL);
    rv = 1000 * tv.tv_sec;
    rv += tv.tv_usec / 1000;
    return rv;
}

FAKE_VALUE_FUNC(Stream *, myFopen, const char *);
Stream *myFopen_value_fake(const char *) {
    Stream *s = (Stream *) myMalloc(sizeof(Stream));
    return s;
}

FAKE_VALUE_FUNC(int, svc_myFputs, const char *, Stream *);
FAKE_VOID_FUNC(svc_myFflush);

FAKE_VOID_FUNC(disable_interrupts);
FAKE_VOID_FUNC(enable_interrupts);
FAKE_VOID_FUNC(close_all_streams_for_pid, uint32_t);

FAKE_VOID_FUNC(svc_block);
FAKE_VOID_FUNC(svc_myKill);
FAKE_VOID_FUNC(svc_yield);

FAKE_VALUE_FUNC(int, dummy_main, int, char **);

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
    cleanup_memory();
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
    RESET_FAKE(svc_get_current_millis);
    RESET_FAKE(get_current_millis);

    RESET_FAKE(myFopen);

    RESET_FAKE(svc_myFputs);
    RESET_FAKE(svc_myFflush);

    RESET_FAKE(disable_interrupts);
    RESET_FAKE(enable_interrupts);
    RESET_FAKE(close_all_streams_for_pid);

    RESET_FAKE(svc_block);
    RESET_FAKE(svc_myKill);
    RESET_FAKE(svc_yield);

    RESET_FAKE(dummy_main);

    FFF_RESET_HISTORY();

    myFopen_fake.custom_fake = myFopen_value_fake;
    initialize_PCB_LIST();
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
    destroy_PCB_LIST();
  }
};


TEST_F(ProcessTest, SpawnInsertPCB) {
    uint32_t reference_id = next_process_id();
    struct PCB *iter;
    struct PCB *p, *q;
    uint32_t pid_p, pid_q;

    pid_p = spawn_process(dummy_main);
    p = find_pcb(pid_p);

    EXPECT_EQ(reference_id + 1, p->PID);
    EXPECT_EQ(0, p->total_cpu_time);
    EXPECT_EQ(READY, p->state);

    EXPECT_EQ((get_PCB_LIST())->next, p);

    pid_q = spawn_process(dummy_main);
    q = find_pcb(pid_q);
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

    myKill(pid_p);
    myKill(pid_q);
}

TEST_F(ProcessTest, FindPCB) {
    struct PCB *p, *q;
    uint32_t pid_p;
    pid_p = spawn_process(dummy_main);
    p = find_pcb(pid_p);
    q = find_pcb(pid_p);
    EXPECT_EQ(p, q);
    q = find_pcb(999);
    EXPECT_EQ(NULL, q);
    myKill(pid_p);
}

TEST_F(ProcessTest, myKill) {
    struct PCB *p = create_pcb();
    uint32_t pid = p->PID;
    myKill(pid);
    EXPECT_EQ(NULL, find_pcb(pid));
}

TEST_F(ProcessTest, SpawnKillProcess) {
    struct PCB *p;
    uint32_t pid_p;

    svc_get_current_millis_fake.custom_fake = svc_get_current_millis_value_fake;
    get_current_millis_fake.custom_fake = svc_get_current_millis_value_fake;
    uint64_t pre_start_millis = svc_get_current_millis();
    pid_p = spawn_process(dummy_main);
    uint64_t post_start_millis = svc_get_current_millis();

    p = find_pcb(pid_p);
    EXPECT_LE(pre_start_millis, p->start_time_millis);
    EXPECT_GE(post_start_millis, p->start_time_millis);

    uint64_t pre_end_millis = svc_get_current_millis();
    myKill(p->PID);
    uint64_t post_end_millis = svc_get_current_millis();

    EXPECT_EQ(KILLED, p->state);
    EXPECT_LE(pre_end_millis, p->end_time_millis);
    EXPECT_GE(post_end_millis, p->end_time_millis);

    uint64_t cpu_time_lower_bound = pre_end_millis - post_start_millis;
    uint64_t cpu_time_upper_bound = post_end_millis - pre_start_millis;
    EXPECT_LE(cpu_time_lower_bound, p->total_time_millis);
    EXPECT_GE(cpu_time_upper_bound, p->total_time_millis);
}

TEST_F(ProcessTest, ChooseProcessToRun) {
    uint32_t pid_p, pid_q, pid_r;
    struct PCB *p, *q, *r, *chosen;
    pid_p = spawn_process(dummy_main);
    p = find_pcb(pid_p);
    pid_q = spawn_process(dummy_main);
    q = find_pcb(pid_q);
    pid_r = spawn_process(dummy_main);
    r = find_pcb(pid_r);

    p->state = BLOCKED;
    r->state = BLOCKED;

    chosen = choose_process_to_run();
    EXPECT_EQ(q, chosen);

    q->state = BLOCKED;
    p->state = READY;
    chosen = choose_process_to_run();
    EXPECT_EQ(p, chosen);

    p->state = BLOCKED;
    r->state = READY;
    chosen = choose_process_to_run();
    EXPECT_EQ(r, chosen);
}

TEST_F(ProcessTest, CreateStack) {
    struct PCB *p;
    int result;
    p = create_pcb();
    result = create_stack(p);
    EXPECT_EQ(SUCCESS, result);
    EXPECT_EQ(STACK_SIZE, p->stack_size);
    EXPECT_NE((void *) NULL, p->stack_pointer);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

