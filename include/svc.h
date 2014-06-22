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

// Implemented SVC numbers
#define SVC_FREE 0
#define SVC_MALLOC 1
#define SVC_FPUTC 2
#define SVC_FPUTS 3
#define SVC_FGETC 4
#define SVC_FGETS 5
#define SVC_FOPEN 6
#define SVC_FCLOSE 7
#define SVC_CREATE 8
#define SVC_DELETE 9
#define SVC_GET_CURRENT_MILLIS 10
#define SVC_GETTIMEOFDAY 11
#define SVC_SETTIMEOFDAY 12
#define SVC_SETTIMER 13
#define SVC_FLUSHOUTPUT 14
#define SVC_SPAWN 15
#define SVC_YIELD 16
#define SVC_BLOCK 17
#define SVC_WAKE 18
#define SVC_KILL 19


void svcInit(void);
void svcInit_SetSVCPriority(unsigned char priority);
void svcHandler(void);
void svcPushState(void);
void svcPopState(void);

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
int svc_myFgetc(Stream *stream);
char *svc_myFgets(char *str, int size, Stream *stream);

uint64_t svc_get_current_millis(void);
int svc_gettimeofday(struct timeval *tp, void *tzp);
int svc_settimeofday(const struct timeval *tp, const struct timezone *tzp);

void svc_setTimer(uint16_t count);
int svc_myFflush(Stream *stream);

int svc_spawn(void);
void svc_yield(void);
void svc_block(void);
void svc_wake(uint16_t pid);
void svc_myKill(uint16_t pid);

#endif /* ifndef _SVC_H */
