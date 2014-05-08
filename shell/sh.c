#if !defined IO_DEMO && !defined SVC_DEMO

#include "shell.h"

int main(void) {
    initialize_system();
    run_shell();
    return 0;
}

#endif
