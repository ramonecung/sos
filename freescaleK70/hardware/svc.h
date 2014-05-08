/**
 * svc.h
 * Routines for supervisor calls
 *
 * ARM-based K70F120M microcontroller board
 *   for educational purposes only
 * CSCI E-92 Spring 2014, Professor James L. Frankel, Harvard Extension School
 *
 * Written by James L. Frankel (frankel@seas.harvard.edu)
 */

#include "../io.h"

#ifndef _SVC_H
#define _SVC_H

#define SVC_MaxPriority 15
#define SVC_PriorityShift 4

// Implemented SVC numbers
#define SVC_FREE 0
#define SVC_MALLOC 1
#define SVC_FPUTC 2
#define SVC_FGETC 3
#define SVC_FOPEN 4
#define SVC_FCLOSE 5
#define SVC_CREATE 6
#define SVC_DELETE 7

void svcInit_SetSVCPriority(unsigned char priority);
void svcHandler(void);

void svc_myFree(void *ptr);
void *svc_myMalloc(unsigned int size);
int svc_myFputc(int c, Stream *stream);
int svc_myFgetc(Stream *stream);
Stream *svc_myFopen(const char *filename);
int svc_myFclose(Stream *stream);
int svc_myCreate(const char *filename);
int svc_myDelete(const char *filename);

#endif /* ifndef _SVC_H */
