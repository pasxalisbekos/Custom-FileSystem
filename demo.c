
#include "custom_filesys.h"

void write_to_file(char* filename){
    
    int fd = open(filename, O_CREAT | O_WRONLY, 0644);
    if (fd == -1) {
        perror("Error opening file");
        // return 1;
    }

    const char *str = "Hello, this is a custom write function!";
    ssize_t bytes_written = my_write(filename, fd, str, strlen(str), getpid());
    if (bytes_written == -1) {
        perror("Error writing to file");
        close(fd);
    }

    close(fd);
    
}

int main() {



    char *filename = "./dir_a/dir_b/dir_c/example.txt";
    char *file_1 = "/home/pbekos/pass.txt";
    char *file_2 = "/home/pbekos/fail.txt";
    char *file_3 = "/home/pbekos/Documents/hw2-master/hw2/dtmf_in.txt";
    char *file_4 = "/home/pbekos/Desktop/100_websites_no_consent_fbpixel.txt";
    char *file_5 = "/home/pbekos/Desktop/rerun/grader-s19/requirements.txt";
    char *file_6 = "/home/pbekos/Desktop/test12/test.txt";
    int fd = open(filename, O_CREAT | O_WRONLY, 0644);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    const char *str = "Hello, this is a custom write function!";
    ssize_t bytes_written = my_write(filename, fd, str, strlen(str), getpid());
    if (bytes_written == -1) {
        perror("Error writing to file");
        close(fd);
        return 1;
    }

    close(fd);


    write_to_file(file_1);
    write_to_file(file_2);
    write_to_file(file_3);
    write_to_file(file_4);
    write_to_file(file_5);
    write_to_file(file_6);

    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file for reading");
        return 1;
    }

    char buffer[100];
    ssize_t bytes_read = my_read(filename, fd, buffer, sizeof(buffer), getpid());
    if (bytes_read == -1) {
        perror("Error reading from file");
        close(fd);
        return 1;
    }

    printf("Read from file: %s\n", buffer);

    close(fd);

    print_tree_recursive(directory_tree_head, 0);


    return 0;
}
