#include "operational.h"
#define BUFFER_SIZE 4096


void update_curr_path(char* new_path){
    current_path = strdup(new_path);
}


char** read_operations(char* operation_file_path) {
    FILE *file = fopen(operation_file_path, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open file %s\n", operation_file_path);
        exit(EXIT_FAILURE);
    }

    // Allocate initial memory for lines array
    int capacity = 10;  // Initial capacity
    int size = 0;        // Number of lines read
    char **lines = malloc(capacity * sizeof(char*));
    if (lines == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        // Remove newline character from the end of the line
        buffer[strcspn(buffer, "\n")] = '\0';

        // Allocate memory for the line and copy it
        lines[size] = malloc((strlen(buffer) + 1) * sizeof(char));
        if (lines[size] == NULL) {
            fprintf(stderr, "Error: Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        strcpy(lines[size], buffer);

        // Increase size and expand capacity if needed
        size++;
        if (size >= capacity) {
            capacity *= 2;  // Double the capacity
            char **temp = realloc(lines, capacity * sizeof(char*));
            if (temp == NULL) {
                fprintf(stderr, "Error: Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
            lines = temp;
        }
    }

    // Close file
    fclose(file);

    // Resize lines array to fit actual number of lines
    char **temp = realloc(lines, (size + 1) * sizeof(char*));
    if (temp == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    lines = temp;

    // Add NULL terminator to indicate end of lines array
    lines[size] = NULL;
    return lines;
}

operation* parse_operation(char* line) {
    operation* op_info = malloc(sizeof(operation));
    if (op_info == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Tokenize the line based on "|"
    char* token = strtok(line, "|");
    if (token != NULL) {
        // Extract the operation
        strcpy(op_info->op, token);

        // Extract the source path
        token = strtok(NULL, "|");
        if (token != NULL) {
            strcpy(op_info->source, token);
        }

        // Extract the destination path
        token = strtok(NULL, "|");
        if (token != NULL) {
            strcpy(op_info->destination, token);
        }
    }

    return op_info;

}

void print_operations(char** lines){
    for (int i = 0; lines[i] != NULL; i++) {
        // printf("%s\n", lines[i]);
        operation* info = parse_operation(lines[i]);
        printf("Operation: %s, Source: %s, Destination: %s\n", info->op, info->source, info->destination);

    }
}



void signal_handler(int signum) {
    printf("Caught signal %d\n", signum);
    exit(signum);
}

void custom_signal_handler(int signum) {
    printf("Caught signal %d| Operation path: %s\n", signum, current_path);
    //after here we have to find the nodes and release their locks
    exit(signum);
}

void handler_init(){
    // Setting up our signal handler for all those signals
    signal(SIGKILL, custom_signal_handler); // process killed
    signal(SIGSEGV, custom_signal_handler); // Segmentation fault
    signal(SIGABRT, custom_signal_handler); // Abort signal
    signal(SIGILL,  custom_signal_handler);  // Illegal instruction
    signal(SIGFPE,  custom_signal_handler);  // Floating point exception

}





void copy_file(char *source_file,char *destination_file) {
    // Open the source file for reading
    int source_fd = open(source_file, O_RDONLY);
    if (source_fd == -1) {
        perror("Failed to open source file");
        return;
    }

    // Open the destination file for writing, create it if it doesn't exist
    int destination_fd = open(destination_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (destination_fd == -1) {
        perror("Failed to open destination file");
        close(source_fd);
        return;
    }

    // Read from source and write to destination
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;

    while ((bytes_read = read(source_fd, buffer, BUFFER_SIZE)) > 0) {
        bytes_written = my_write(destination_file, destination_fd, buffer, bytes_read, (int)getpid());
        if (bytes_written != bytes_read) {
            perror("Write error");
            close(source_fd);
            close(destination_fd);
            return;
        }
    }

    if (bytes_read == -1) {
        perror("Read error");
    }

    // Close files
    close(source_fd);
    close(destination_fd);
}
