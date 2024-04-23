#ifndef OPERATIONAL_H
#define OPERATIONAL_H

// #include "custom_filesys.h"
#include <signal.h>
#include <pthread.h>

// The current path to the file an operation takes place
static char* current_path;

void update_curr_path(char* new_path);
void handler_init();
void custom_signal_handler(int signum);



// Each thread has one instance of that node with its PID. When all threads are complete then we should create the 
// JSON tree file
typedef struct thread_node{
    int PID;
    struct thread_node* next;
} thread_node;


// This struct will be used only to preserve consistency when modifying the threads list
typedef struct threads_list {
    struct thread_node* head;
    pthread_mutex_t mutex;
} threads_list;


void insert_thread(threads_list** list, int PID);
void remove_thread_node(threads_list** list, int PID);
threads_list* get_linked_list_instance();

extern threads_list* global_head;

void init(int PID);
void end(int PID);




#endif