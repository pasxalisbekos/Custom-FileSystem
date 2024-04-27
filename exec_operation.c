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
    my_write(filename, fd, (const char*)buf, strlen(buf), getpid(), mode);
}

void write_to_file(char* filename, int flag){
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

    const char *str = "Hello, this is a custom write function!\n";
    ssize_t bytes_written = my_write_temp(filename, fd, str, strlen(str), getpid(), (flag == 1) ? APPEND : WRITE);
    if (bytes_written == -1) {
        perror("Error writing to file");
    }
    close(fd);
}

void read_from_file(char* filename){


    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file for reading");
        return 1;
    }

    char buffer[100];
    ssize_t bytes_read = my_read(filename, fd, buffer, sizeof(buffer), getpid(), 0);
    if (bytes_read == -1) {
        perror("Error reading from file");
        close(fd);
        return 1;
    }

    close(fd);

}

int main(int argc, char *argv[]) {
    
    handler_init();

    if (argc == 7){
        char* destination = strdup(argv[3]);
        char* new_val = strdup(argv[4]);
        int operation_flag = atoi(argv[5]);
        int delay = atoi(argv[6]);
        printf("Process with PID: [%d] and random delay {%d} will perform operation [%d] to file: %s the value: %s\n",getpid(),delay,operation_flag,destination,new_val);
        // sleep(delay);
        write_to_file(destination,operation_flag);
    }   
    else if (argc == 4){
        char* destination = strdup(argv[2]);
        int delay = atoi(argv[3]);
        printf("Process with PID: [%d] and random delay {%d} will perform read from file: %s\n",getpid(),delay,destination);
        // sleep(delay);
        read_from_file(destination);
    }
    else{
        printf("Unknown arguments\n Exiting...\n");
        return 0;
    }
    end();

    return 0;

}
