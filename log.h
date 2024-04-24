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
#include <sys/file.h>


#define LOG_FILE "/home/snapshots/log.txt"


void init_spinlock();
void destroy_spinlock();
void acquire_spinlock();
void release_spinlock();
void log_write(int pid, char* file_path);



#define ABSOLUTE_PATHS "/home/snapshots/absolute_paths.txt"
#define MAX_LINES 100
#define MAX_LINE_LENGTH 256
#define MAX_LINE_LENGTH 1024

void init_spinlock_dir_tree();
void destroy_spinlock_dir_tree();
void acquire_spinlock_dir_tree();
void release_spinlock_dir_tree();
void log_write_abs_path(char* file_path,int type);
char** read_absolute_paths(int* line_count);
#endif
