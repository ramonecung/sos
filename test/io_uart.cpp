extern "C" {
#include "../freescaleK70/io.h"
#include "../freescaleK70/io_uart.h"
#include "../memory/memory.h"
#include "../util/util.h"
/* #include "../freescaleK70/hardware/derivative.h" */
#include "../init/init.h"
}

#include "gtest/gtest.h"
#include "../third-party/fff.h"
DEFINE_FFF_GLOBALS;


FAKE_VOID_FUNC(uartInit);

struct UART_MemMap {
    char fake;
};
typedef struct UART_MemMap * UART_MemMapPtr;

FAKE_VOID_FUNC(uartPutchar, UART_MemMapPtr, char);
FAKE_VALUE_FUNC(char, uartGetchar, UART_MemMapPtr);
FAKE_VALUE_FUNC(int, system_initialized);


class IOUartTest : public ::testing::Test {
  protected:

  // You can remove any or all of the following functions if its body
  // is empty.

  Stream *test_stream;
  Stream ts;

  IOUartTest() {
    // You can do set-up work for each test here.
    initialize_memory();
  }

  virtual ~IOUartTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
    RESET_FAKE(system_initialized);
    RESET_FAKE(uartInit);
    RESET_FAKE(uartPutchar);
    RESET_FAKE(uartGetchar);

    FFF_RESET_HISTORY();

    ts.device_instance = UART2;
    test_stream = &ts;
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
};

TEST_F(IOUartTest, InitializeIOUart) {
  initialize_io_uart();
  EXPECT_EQ(1, uartInit_fake.call_count);
}

TEST_F(IOUartTest, Fgetc) {
    uartGetchar_fake.return_val = 'c';
    EXPECT_EQ('c', fgetc_uart(test_stream));
    EXPECT_EQ(1, uartGetchar_fake.call_count);
}

TEST_F(IOUartTest, Fputc) {
    int c, res;
    c = 'c';
    res = fputc_uart(c, test_stream);
    EXPECT_EQ(c, res);
    EXPECT_EQ(1, uartPutchar_fake.call_count);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
