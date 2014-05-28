#ifdef TOUCH_PAD_DEMO

#include "../include/io.h"
#include "../include/svc.h"
#include "../init/init.h"
#include "../util/util.h"

int main(void) {
    Stream *sbutton, *s1, *s2, *s3, *s4, *sledo, *sledy, *sledg, *sledb;

	initialize_system();
    efputs("CapacitivePads Project Starting\r\n", STDOUT);

    sbutton = svc_myFopen("/dev/button/sw2");
    s1 = svc_myFopen("/dev/touch/e1");
    s2 = svc_myFopen("/dev/touch/e2");
    s3 = svc_myFopen("/dev/touch/e3");
    s4 = svc_myFopen("/dev/touch/e4");
    sledo = svc_myFopen("/dev/led/orange");
    sledy = svc_myFopen("/dev/led/yellow");
    sledg = svc_myFopen("/dev/led/green");
    sledb = svc_myFopen("/dev/led/blue");

    while(!svc_myFgetc(sbutton)) {
        if(svc_myFgetc(s1)) {
            svc_myFputc(1, sledo);
        } else {
            svc_myFputc(0, sledo);
        }
        if(svc_myFgetc(s2)) {
            svc_myFputc(1, sledy);
        } else {
            svc_myFputc(0, sledy);
        }
        if(svc_myFgetc(s3)) {
            svc_myFputc(1, sledg);
        } else {
            svc_myFputc(0, sledg);
        }
        if(svc_myFgetc(s4)) {
            svc_myFputc(1, sledb);
        } else {
            svc_myFputc(0, sledb);
        }
    }

    efputs("CapacitivePads Project Completed\r\n", STDOUT);

    return 0;
}
#endif

