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
#include <openssl/sha.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cjson/cJSON.h> 
#include <pwd.h>
#include <grp.h>
#include <signal.h>
#include <pthread.h>


#include "cleanup_daemon.h"
#include "log.h"



#define MAX_TIMESTAMP_LENGTH 20
#define SHA256_DIGEST_LENGTH 32
#define MAX_SPIN_ATTEMPTS 10000

void handler_init();
void custom_signal_handler(int signum);
void end();



typedef enum operation_type {
    READ = 0,
    WRITE = 1,
    APPEND = 2
} operation_type;

typedef struct snapshot {
    
    char* snapshot_path;
    char* timestamp;
    char* operation;
    int PID;
    struct snapshot* next;

} snapshot;


typedef struct file_node {
    
    char* file_name;
    char* file_hash;
    char* absolute_path;
    char* versions_path;
    char* permissions;
    long size;
    char* ownership;
    char* operations;
    size_t operations_size;
    struct snapshot* snapshot;
    struct file_node* next;
    
} file_node;

typedef struct dir_node {

    char* dir_name;
    char* dir_hash;
    struct dir_node** children;
    file_node* files_list;

} dir_node;

// The head of the directory tree
extern dir_node* directory_tree_head;


ssize_t my_write(char* filepath, int fd, const void *buf, size_t count, int PID, operation_type type);
ssize_t my_read(char* filepath, int fd, void *buf, size_t count, int PID, operation_type type);
// --------------------------------------------------------------------- HELPER FUNCTIONS --------------------------------------------------------------------- //
char* get_last_element(char** array);
char** str_split(char* file_path, const char delimeter);
char *append_strings(char *start, char *add);
char *sha256(const char *input);
char* current_timestamp_as_string();
char* replace_char(char* str, char find, char replace);
char *get_permissions(char *path_to_file);
long get_file_size(char *path_to_file);
char *get_file_ownership(char *path_to_file);
char* real_path(char* filepath);
// --------------------------------------------------------------------- FILE LIST FUNCTIONS --------------------------------------------------------------------- //
void push(file_node** head_ref, char* file_name, char* absolute_path,int PID, int flag, int operation);
void update_only_snapshot(snapshot** head, char* file_name, char* absolute_path, int PID, int operation);
void print_list(file_node* head);
char* create_snapshot(char* file_name, char* absolute_path, char* timestamp);
file_node* search_for_file_node(file_node* head, char* file_name);
// --------------------------------------------------------------------- DIRECTORY FUNCTIONS --------------------------------------------------------------------- //
void add_child(dir_node* parent, dir_node* child);
void add_directory_to_tree(char* dir_name,int PID, int flag, int operation);
dir_node* create_dir_node(const char* dir_name);
void print_tree_recursive(dir_node* node, int depth);
// -------------------------------------------------------------------- TESTING PRESERVING TREE TO JSON --------------------------------------------------------- //
cJSON* dir_tree_to_json(dir_node* root);
void write_json_to_file(cJSON* json, const char* filename);
void construct_tree(char* file_path, int PID);
void collect_absolute_paths(cJSON* json_node, char*** paths, int* count);
char** get_absolute_paths_from_json(char* filename, int* count);
int try_to_access_json(int PID);
int extract_tree_to_json();
// -------------------------------------------------------------------- LOCKING JSON TREE FILE ------------------------------------------------------------------- //
void init_spinlock_json_tree();
void destroy_spinlock_json_tree();
void acquire_spinlock_json_tree();
void release_spinlock_json_tree();
void tree_write();
#endif /* WRAPPER_LIBRARY_H */
