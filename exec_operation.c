#include <stdio.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
 #include <sys/stat.h>
#include "custom_filesys.h"


void write_to_file(char* filename,char* new_val){
    
    int fd = open(filename, O_CREAT | O_WRONLY, 0777);
    if (fd == -1) {
        perror("Error opening file");
        // return 1;
    }

    char *str = "Hello, this is a custom write function!";
    ssize_t bytes_written = my_write(filename, fd, (const char*)new_val, strlen(new_val), getpid(),WRITE);
    if (bytes_written == -1) {
        perror("Error writing to file");
        close(fd);
    }

    close(fd);
    
}


int main(int argc, char *argv[]) {
    
    // init(getpid());

    srand((unsigned int)time(NULL));
    int delay = rand() % 11; // rand() % (max + 1)
    // printf("%s\n",argv[4]);
    if (argc == 5){
        char* destination = strdup(argv[3]);
        char* new_val = strdup(argv[4]);
        // printf("Process with PID: [%d] and random delay {%d} will write to file: %s the value: %s\n",getpid(),delay,destination,new_val);
        // sleep(delay);
        write_to_file(destination,new_val);
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
