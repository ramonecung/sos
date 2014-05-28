#include "../include/constants.h"
#include "../include/io.h"
#include "ser2lcd.h"
#include "../include/svc.h"
#include "../util/util.h"

#ifdef TEST_SHELL
int cmd_ser2lcd(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_ser2lcd(int argc, char *argv[]) {
#endif
    int res;
    Stream *s1, *s2;
    int c;
    s1 = svc_myFopen("/dev/uart/uart2");
    s2 = svc_myFopen("/dev/lcd/lcd");
    /* Continuously copy characters from serial input to LCD. */
    while(1) {
        c = svc_myFgetc(s1);
        if (c == EOF) {
            res = efputs("ser2lcd: end of file\r\n", ostrm);
            return READ_ERROR;
        }
        res = svc_myFputc(c, s2);
        if (res == EOF) {
            res = efputs("ser2lcd: error writing to LCD\r\n", ostrm);
            return WRITE_ERROR;
        }
        /* End on a ^D (control-D) input character. */
        if(c == CHAR_EOF) {
            res = efputs("\r\n", ostrm);
            return SUCCESS;
        }
    }
}
