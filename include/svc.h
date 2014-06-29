/**
 * svc.h
 * Routines for supervisor calls
 *
 * ARM-based K70F120M microcontroller board
 *   for educational purposes only
 * CSCI E-92 Spring 2014, Professor James L. Frankel, Harvard Extension School
 *
 * Written by James L. Frankel (frankel@seas.harvard.edu) and Stephen Karger
 */

#include "io.h"
#include <stdint.h>
#ifndef TEST_SHELL
#include "../time/time.h"
#else
#include <sys/time.h>
#endif

#ifndef _SVC_H
#define _SVC_H

#define SVC_MaxPriority 15
#define SVC_PriorityShift 4

/* Lowest priority (15): SVC instruction */
#define SVC_Priority 15

void svcInit(void);
void svcInit_SetSVCPriority(unsigned char priority);
void svcHandler(void);

/* memory allocation */
void *svc_myMalloc(unsigned int size);
void svc_myFree(void *ptr);

/* Device Independent IO */
int svc_myCreate(const char *filename);
int svc_myDelete(const char *filename);

Stream *svc_myFopen(const char *filename);
int svc_myFclose(Stream *stream);

int svc_myFputc(int c, Stream *stream);
int svc_myFputs(const char *s, Stream *stream);
int svc_myFflush(Stream *stream);
int svc_myFgetc(Stream *stream);
char *svc_myFgets(char *str, int size, Stream *stream);

/* time */
uint64_t svc_get_current_millis(void);
int svc_gettimeofday(struct timeval *tp, void *tzp);
int svc_settimeofday(const struct timeval *tp, const struct timezone *tzp);
void svc_setTimer(uint32_t count, void (*)(void));

/* process */
int svc_spawn(int (*mainfunc)(int argc, char **argv));
void svc_yield(void);
void svc_block(void);
void svc_wake(uint32_t pid);
void svc_myKill(uint32_t pid);

#endif /* ifndef _SVC_H */
