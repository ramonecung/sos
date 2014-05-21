# Supervisor Calls
===

## Document for a systems programmer that describes how to invoke each of the supervisor calls implemented in SOS.

SOS defines several C functions that provide an interface to system services, such as opening a file or allocating memory. These functions follow the naming convention svc_*

Include freescaleK70/hardware/svc.h to use these functions in an application. (Header location may change in the future.)

As declared in svc.h, the supervisor calls are:

### /* memory allocation */
- void *svc_myMalloc(unsigned int size);
- void svc_myFree(void *ptr);

### /* Device Independent IO */

- int svc_myCreate(const char *filename);
- int svc_myDelete(const char *filename);
- Stream *svc_myFopen(const char *filename);
- int svc_myFclose(Stream *stream);
- int svc_myFputc(int c, Stream *stream);
- int svc_myFputs(const char *s, Stream *stream);
- int svc_myFgetc(Stream *stream);
- char *svc_myFgets(char *str, int size, Stream *stream);
- int svc_gettimeofday(struct timeval *tp, void *tzp);
- int svc_settimeofday(const struct timeval *tp, const struct timezone *tzp);




## Design document that describes the way that supervisor calls are implemented.

Note: this design mirrors the one in the course provided svc.c / svc.h.

The file svc.c defines the svc\_\* calls. These functions issue the "svc"
assembly instruction. svc takes an immediate constant as an argument,
and each svc_* function passes a constant corresponding to the
desired supervisor call. Those constants are #defined in svc.h.

svc.c explains the multiple events that happen when the svc assembly
instruction occurs. In particular:
 The PC is set to the address in the interrupt vector table for
 the interrupt service routine for the SVC instruction.

kinetis_sysinit.c defines the InterruptVector table. The interrupt service
routine appears at index 11, and for SOS it has the function svcHandler.
So svcHandler executes in the response to the svc instruction.

svc.c defines svcHandler. First this function sets R0 to the top of stack,
which is the top of the stack frame that the processor pushed upon
executing the svc assembly instruction.
Then svcHandler branches to svcHandlerInC. The implication is that
R0 will be the argument to svcHandlerInC.

Using the fact that it has a reference to the top of stack in R0,
svcHandlerInC can use its argument to refer to words on the svc stack frame.
In particular, the svc stack has a return address word, which has
the location of the next instruction to execute after the SV call completes.
The instruction right before that location is the svc instruction itself.

The svc instruction including its argument is 2 bytes long, and the argument part
is located in the first byte, so the address of the argument value is
(return address - 2 bytes). The argument value is the #defined
supervisor call constant.

svcHandlerInC maintains a switch statement with cases for each of the
 #defined supervisor call constants. The switch statement dispatches
to the "implementation" versions of each supervisor call, i.e. the
functions defined in C that perform the actual work for that service.

Relying on the fact that the svc instruction causes the processor
to switch to Handler mode, the implementation functions execute
in privileged mode and have no restrictions.
