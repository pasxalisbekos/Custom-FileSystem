
#include "custom_filesys.h"
#include "cleanup_daemon.h"
#include "log.h"
#include "operational.h"

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
        fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0777);
    } else {
        perror("Invalid flag");
        return;
    }
    
    if (fd == -1) {
        perror("Error opening file");
        return;
    }

    const char *str = "Hello, this is a custom write function!";
    ssize_t bytes_written = my_write_temp(filename, fd, str, strlen(str), getpid(), (flag == 1) ? APPEND : WRITE);
    if (bytes_written == -1) {
        perror("Error writing to file");
    }

    close(fd);
}




int main() {

    
    // char** lines = read_operations("./operations.txt");
    // print_operations(lines);
    // monitor_main("./operations.txt");
    // return 0;

    // init_spinlock();

    char *filename = "./dir_a/dir_b/dir_c/example.txt";
    char *filename2 = "./dir_a/dir_b/dir_c/example2.txt";
    char *file_1 = "/home/pbekos/pass.txt";
    char *file_2 = "/home/pbekos/fail.txt";
    char *file_3 = "/home/pbekos/Documents/hw2-master/hw2/dtmf_in.txt";
    char *file_4 = "/home/pbekos/Desktop/100_websites_no_consent_fbpixel.txt";
    char *file_5 = "/home/pbekos/Desktop/rerun/grader-s19/requirements.txt";
    char *file_6 = "/home/pbekos/Desktop/test12/test.txt";
    // int fd = open(filename, O_CREAT | O_WRONLY, 0644);
    // if (fd == -1) {
    //     perror("Error opening file");
    //     return 1;
    // }

    // const char *str = "Hello, this is a custom write function!";
    // ssize_t bytes_written = my_write(filename, fd, str, strlen(str), getpid());
    // if (bytes_written == -1) {
    //     perror("Error writing to file");
    //     close(fd);
    //     return 1;
    // }

    // close(fd);
    // // A series of write operations on different files to verify the directory tree creation, the snapshot preservation 
    // // and the module that saves the structures into a JSON file so we can later (after a system crash) reconstruct the tree
    write_to_file(file_1,1);
    // sleep(5);
    write_to_file(file_2,0);
    // sleep(3);
    write_to_file(file_5,1);
    // sleep(10);
    write_to_file(file_3,1);
    write_to_file(filename2,1);
    // sleep(9);
    write_to_file(file_4,0);
    // sleep(1);
    write_to_file(file_5,0);
    // sleep(8);
    write_to_file(file_2,1);
    // sleep(2);
    write_to_file(file_6,1);
    write_to_file(filename,1);
    write_to_file(filename,1);
    write_to_file(file_1,0);
    // sleep(5);
    write_to_file(file_2,1);
    // sleep(3);
    write_to_file(file_5,1);
    // sleep(10);
    write_to_file(file_3,0);
    // sleep(9);
    write_to_file(file_4,1);
    // sleep(1);
    write_to_file(file_5,0);
    // sleep(8);
    write_to_file(file_2,1);
    write_to_file(filename2,0);
    // sleep(2);
    write_to_file(file_6,1);
    write_to_file(filename,1);
    sleep(2);
    write_to_file(filename,1);
    sleep(2);
    write_to_file(filename,0);
    sleep(2);
    write_to_file(filename,0);
    sleep(2);
    write_to_file(filename,1);
    sleep(2);
    write_to_file(filename,1);

    // fd = open(filename, O_RDONLY);
    // if (fd == -1) {
    //     perror("Error opening file for reading");
    //     return 1;
    // }

    // char buffer[100];
    // ssize_t bytes_read = my_read(filename, fd, buffer, sizeof(buffer), getpid());
    // if (bytes_read == -1) {
    //     perror("Error reading from file");
    //     close(fd);
    //     return 1;
    // }

    // // printf("Read from file: %s\n", buffer);

    // close(fd);

    // // list_sub_directories("/home/snapshots");

    // destroy_spinlock();

    // int y = try_to_access_json(getpid());

    int x = extract_tree_to_json();


    // list_sub_directories("/home/snapshots");
    return 0;
}
