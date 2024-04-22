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


#define MAX_TIMESTAMP_LENGTH 20
#define SHA256_DIGEST_LENGTH 32
#define MAX_SPIN_ATTEMPTS 10000

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


ssize_t my_write(char* filepath, int fd, const void *buf, size_t count, int PID);
ssize_t my_read(char* filepath, int fd, void *buf, size_t count, int PID);

// --------------------------------------------------------------------- HELPER FUNCTIONS --------------------------------------------------------------------- //
char* get_last_element(char** array);
char** str_split(char* file_path, const char delimeter);
char *append_strings(char *start, char *add);
char *sha256(const char *input);
char* current_timestamp_as_string();
char* replace_char(char* str, char find, char replace);
// --------------------------------------------------------------------- FILE LIST FUNCTIONS --------------------------------------------------------------------- //
void push(file_node** head_ref, char* file_name, char* absolute_path,int PID);
void update_only_snapshot(snapshot** head, char* file_name, char* absolute_path, int PID);
void print_list(file_node* head);
char* create_snapshot(char* file_name, char* absolute_path, char* timestamp);
file_node* search_for_file_node(file_node* head, char* file_name);
// --------------------------------------------------------------------- DIRECTORY FUNCTIONS --------------------------------------------------------------------- //
void add_child(dir_node* parent, dir_node* child);
void add_directory_to_tree(char* dir_name,int PID);
dir_node* create_dir_node(const char* dir_name);
void print_tree_recursive(dir_node* node, int depth);
// -------------------------------------------------------------------- TESTING PRESERVING TREE TO JSON ----------------------------------------------------
cJSON* dir_tree_to_json(dir_node* root);
void write_json_to_file(cJSON* json, const char* filename);
void construct_tree(char* file_path, int PID);
void collect_absolute_paths(cJSON* json_node, char*** paths, int* count);
char** get_absolute_paths_from_json(char* filename, int* count);
int try_to_access_json(int PID);
int extract_tree_to_json();


#endif /* WRAPPER_LIBRARY_H */
