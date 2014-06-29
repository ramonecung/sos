#ifndef _SVC_INTERNAL_H
#define _SVC_INTERNAL_H

void *internal_svc_myMalloc(unsigned int size);
void internal_svc_myFree(void *ptr);

int internal_svc_myCreate(const char *filename);
int internal_svc_myDelete(const char *filename);

Stream *internal_svc_myFopen(const char *filename);
int internal_svc_myFclose(Stream *stream);

int internal_svc_myFputc(int c, Stream *stream);
int internal_svc_myFputs(const char *s, Stream *stream);
int internal_svc_myFflush(Stream *stream);
int internal_svc_myFgetc(Stream *stream);
char *internal_svc_myFgets(char *str, int size, Stream *stream);

uint64_t internal_svc_get_current_millis(void);
int internal_svc_gettimeofday(struct timeval *tp, void *tzp);
int internal_svc_settimeofday(const struct timeval *tp, const struct timezone *tzp);

void internal_svc_setTimer(uint32_t count, void (*)(void));


int internal_svc_spawn(int (*mainfunc)(int argc, char **argv));
void internal_svc_yield(void);
void internal_svc_block(void);
void internal_svc_wake(uint32_t pid);
void internal_svc_myKill(uint32_t pid);

#endif
