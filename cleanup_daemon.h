#ifndef CLEANUP_DAEMON_H
#define CLEANUP_DAEMON_H
#define _GNU_SOURCE

#include "custom_filesys.h"

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

void list_sub_directories(char *base_directory_path);
char* list_files_in_directory(char *directory_path);
char* most_recent_timestamp(char* timestamp1, char* timestamp2);
char* extract_timestamp(char* full_string);
void remove_old_files(char* directory_path, char* most_recent_snapshot);
void init_cleanup_daemon(int flag ,unsigned int time);


#endif
