#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <pthread.h>



#define LOG_FILE "/home/snapshots/log.txt"


void init_spinlock();
void destroy_spinlock();
void acquire_spinlock();
void release_spinlock();
void log_write(int pid, char* file_path);

#endif
