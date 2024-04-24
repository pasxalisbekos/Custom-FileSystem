#include <stdio.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
 #include <sys/stat.h>
#include "custom_filesys.h"

#define WRITE 1
#define APPEND 2



ssize_t my_write_temp(char* filename, int fd, const char *buf, size_t count, pid_t pid, int mode) {
    if (mode == WRITE) {
        lseek(fd, 0, SEEK_SET); // Move to the beginning of the file for writing
    } else if (mode == APPEND) {
        lseek(fd, 0, SEEK_END); // Move to the end of the file for appending
    } else {
        return -1; // Invalid mode
    }
    return my_write(filename, fd, (const char*)buf, strlen(buf), getpid(), mode);
}

void write_to_file(char* filename, char* new_val, int flag){
    int fd;
    if (flag == 1) {

        fd = open(filename, O_CREAT | O_WRONLY | O_APPEND, 0777);
    } else if (flag == 0) {
        fd = open(filename, O_CREAT | O_WRONLY , 0777);
    } else {
        perror("Invalid flag");
        return;
    }
    
    if (fd == -1) {
        perror("Error opening file");
        return;
    }

    // const char *str = "Hello, this is a custom write function!";
    ssize_t bytes_written = my_write_temp(filename, fd, (const char*)new_val, strlen(new_val), getpid(),(flag == 1) ? APPEND : WRITE);
    if (bytes_written == -1) {
        perror("Error writing to file");
    }

    close(fd);
}



int main(int argc, char *argv[]) {
    
    // init(getpid());

    srand((unsigned int)time(NULL));
    int delay = rand() % 11; // rand() % (max + 1)
    // printf("%s\n",argv[4]);
    if (argc == 6){
        char* destination = strdup(argv[3]);
        char* new_val = strdup(argv[4]);
        int operation_flag = atoi(argv[5]);
        printf("Process with PID: [%d] and random delay {%d} will perform operation [%d] to file: %s the value: %s\n",getpid(),delay,operation_flag,destination,new_val);
        // sleep(delay);
        write_to_file(destination,new_val,operation_flag);
        tree_write();

        
    }   
    // Generate a random number between 0 and 10
    // int delay = rand() % 11; // rand() % (max + 1)

    // printf("Sleeping for %d seconds...\n", delay);
    // sleep(delay);

    // printf("Wake up!\n");

    // extract_tree_to_json();
    // end(getpid());
    return 0;
}
