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

#define MAX_TIMESTAMP_LENGTH 20
#define SHA256_DIGEST_LENGTH 32

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
void push(file_node** head_ref, char* file_name, char* absolute_path);
void print_list(file_node* head);
char* create_snapshot(char* file_name, char* absolute_path, char* timestamp);
file_node* search_for_file_node(file_node* head, char* file_name);
// --------------------------------------------------------------------- DIRECTORY FUNCTIONS --------------------------------------------------------------------- //
void add_child(dir_node* parent, dir_node* child);
void add_directory_to_tree(char* dir_name);
dir_node* create_dir_node(const char* dir_name);
void print_tree_recursive(dir_node* node, int depth);


#endif /* WRAPPER_LIBRARY_H */
