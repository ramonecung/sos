#include "../include/constants.h"
#include "../util/util.h"
#include "../init/init.h"
#include "hardware/uart.h"
#include "hardware/intSerialIO.h"
#include "io.h"
#include "io_uart.h"

void initialize_io_uart(void) {
    /* On reset (i.e., before calling mcgInit), the processor clocking
     * starts in FEI (FLL Engaged Internal) mode.  In FEI mode and with
     * default settings (DRST_DRS = 00, DMX32 = 0), the MCGFLLCLK, the
     * MCGOUTCLK (MCG (Multipurpose Clock Generator) clock), and the Bus
     * (peripheral) clock are all set to 640 * IRC.  IRC is the Internal
     * Reference Clock which runs at 32 KHz. [See K70 Sub-Family
     * Reference Manual, Rev. 2, Section 25.4.1.1, Table 25-22 on
     * page 657 and MCG Control 4 Register (MCG_C4) Section 25.3.4 on
     * page 641] */

    /* After calling mcgInit, MCGOUTCLK is set to 120 MHz and the Bus
     * (peripheral) clock is set to 60 MHz.*/

    /* Table 5-2 on page 221 indicates that the clock used by UART0 and
     * UART1 is the System clock (i.e., MCGOUTCLK) and that the clock
     * used by UART2-5 is the Bus clock. */

    const int busClock = 60000000;
    const int KHzInHz = 1000;
    const int busBaud = 115200;

    uartInit(UART2_BASE_PTR, busClock/KHzInHz, busBaud);
    intSerialIOInit();
}

int fputc_uart(int c, Stream *stream) {
    char ch;
    if (stream->device_instance == UART2) {
        ch = (char) c;
        putcharIntoBuffer(ch);
        return ch;
    } else {
        return CANNOT_PUT_CHAR;
    }
}

int fgetc_uart(Stream *stream) {
    if (stream->device_instance == UART2) {
        return getcharFromBuffer();
    }
    return CANNOT_GET_CHAR;
}

void flush_uart(void) {
    flushBuffer();
}
