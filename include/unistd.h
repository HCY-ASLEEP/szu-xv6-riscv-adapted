#ifndef _XV6_UNISTD_H
#define _XV6_UNISTD_H

#include "include/stddef.h"

struct stat;
struct rtcdate;

ssize_t write(int, const void *, size_tt);
ssize_t read(int, void *, size_tt);
int close(int);

int fork(void);
// int exit(void);
int wait(void);
int pipe(int*);
int kill(int);
int exec(char*, char**);
#ifndef _UCC_FCNTL_H
int open(char*, int);
#endif
int mknod(char*, short, short);
int unlink(char*);
int fstat(int fd, struct stat*);
int link(char*, char*);
int mkdir(char*);
int chdir(char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
int halt(void);

#endif  /* unistd.h */
