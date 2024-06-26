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
#include <unistd.h>


void list_sub_directories(char *base_directory_path);
void list_files_in_directory(char *directory_path);
char* most_recent_timestamp(char* timestamp1, char* timestamp2);
char* extract_timestamp(char* full_string);
void remove_old_files(char* directory_path, char* most_recent_snapshot);
void init_cleanup_daemon(int flag ,unsigned int time);
char* get_most_recent_snapshot(char *directory_path);


void get_all_snapshots_for_file(char* file_name);
char* get_snapshot(char* directory, char* snapshot_name);
char* get_most_recent_snapshot_for_file(char* file_path);
int revert_to_previous_version(char* current_file_path, char* most_recent_snapshot);

#endif
