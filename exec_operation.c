#include <stdio.h>
#include <string.h>
#include <time.h>
#include "operational.h"


// void write_to_file(char* filename){
    
//     int fd = open(filename, O_CREAT | O_WRONLY, 0777);
//     if (fd == -1) {
//         perror("Error opening file");
//         // return 1;
//     }

//     const char *str = "Hello, this is a custom write function!";
//     ssize_t bytes_written = my_write(filename, fd, str, strlen(str), getpid());
//     if (bytes_written == -1) {
//         perror("Error writing to file");
//         close(fd);
//     }

//     close(fd);
    
// }
int main(int argc, char *argv[]) {
    
    init(getpid());

    srand((unsigned int)time(NULL));

    // Generate a random number between 0 and 10
    int delay = rand() % 11; // rand() % (max + 1)

    printf("Sleeping for %d seconds...\n", delay);
    sleep(delay);

    printf("Wake up!\n");


    end(getpid());
    return 0;
}
