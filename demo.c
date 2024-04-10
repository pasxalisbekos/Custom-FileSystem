#include "custom_filesys.h"


int main() {



    char *filename = "example.txt";
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

    return 0;
}
