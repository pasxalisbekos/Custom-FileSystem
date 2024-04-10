#ifndef CUSTOM_FILESYS_H
#define CUSTOM_FILESYS_H

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>


// Define your own version of write
ssize_t my_write(char* filepath, int fd, const void *buf, size_t count, int PID);

// Define your own version of read
ssize_t my_read(char* filepath, int fd, void *buf, size_t count, int PID);

#endif /* WRAPPER_LIBRARY_H */
