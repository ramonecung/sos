#ifdef PROCESS_DEMO

#include "../init/init.h"
#include "../process/process.h"
#include "../include/constants.h"

int main(void) {
	struct PCB *p;
	uint16_t pid;
	initialize_system();
	pid = spawn_process();
	p = find_pcb(pid);
	//q = create_process();
	p->state = READY;
	while(TRUE) {
		;
	}
	return 0;
}
#endif
