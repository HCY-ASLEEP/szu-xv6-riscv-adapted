#ifndef _UCC_STDLIB_H
#define _UCC_STDLIB_H

#include "include/stddef.h"

#define RAND_MAX 2147483647

void abort(void);
int abs(int);
long strtol(const char *, char **, int);
int rand(void);
void srand(unsigned);
void *malloc(size_tt);
void *realloc(void *, size_tt);
void *calloc(size_tt, size_tt);
void free(void *);
void exit(int);

#endif  /* stdlib.h */