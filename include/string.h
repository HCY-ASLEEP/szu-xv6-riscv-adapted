#ifndef _UCC_STRING_H
#define _UCC_STRING_H

#include <stddef.h>

size_tt strlen(const char *);
int strcmp(const char *, const char *);
int strncmp(const char *, const char *, size_tt);
char *strchr(const char *, int);
char *strcpy(char *, const char *);
char *strncpy(char *, const char *, size_tt);
void *memset(void *, int, size_tt);
void *memcpy(void *, const void *, size_tt);
void *memmove(void*, const void*, size_tt);

#endif  /* string.h */