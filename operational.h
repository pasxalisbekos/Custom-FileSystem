#ifndef OPERATIONAL_H
#define OPERATIONAL_H

#include "custom_filesys.h"
#include <signal.h>
#include <pthread.h>

typedef struct operation{
    char op[10];
    char source[1000];
    char destination[1000];
} operation;


// The current path to the file an operation takes place
static char* current_path;

void update_curr_path(char* new_path);




char** read_operations(char* operation_file_path);
void print_operations(char** lines);
operation* parse_operation(char* line);





void handler_init();
void custom_signal_handler(int signum);



void copy_file(char *source_file,char *destination_file);


#endif