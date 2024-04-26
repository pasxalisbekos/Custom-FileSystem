
#include "custom_filesys.h"
#include "cleanup_daemon.h"
#include "log.h"


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

    const char *str = "Hello, this is a custom write function!\n";
    ssize_t bytes_written = my_write_temp(filename, fd, str, strlen(str), getpid(), (flag == 1) ? APPEND : WRITE);
    if (bytes_written == -1) {
        perror("Error writing to file");
    }

    close(fd);
}




int main() {

    handler_init();

    // char *filename = "./dir_a/dir_b/dir_c/example.txt";

    // char* recent = get_most_recent_snapshot_for_file(filename); 
    // printf("%s\n",recent);
    // printf("=========================================================\n");
    // get_all_snapshots_for_file(filename);


    // revert_to_previous_version(filename,recent);
    char *filename = "./dir_a/dir_b/dir_c/example.txt";
    char *filename2 = "./dir_a/dir_b/dir_c/example2.txt";
    char *file_1 = "/home/maryam/pass.txt";
    char *file_2 = "/home/maryam/fail.txt";
    char *file_3 = "/home/maryam/Documents/hw2-master/hw2/dtmf_in.txt";
    char *file_4 = "/home/maryam/Desktop/100_websites_no_consent_fbpixel.txt";
    char *file_5 = "/home/maryam/Desktop/rerun/grader-s19/requirements.txt";
    char *file_6 = "/home/maryam/Desktop/test12/test.txt";

    write_to_file(filename,1);
    sleep(1);
    write_to_file(filename,1);
    sleep(1);
    write_to_file(filename,1);
    sleep(1);
    write_to_file(filename,1);
    sleep(1);
    write_to_file(filename,1);
    sleep(1);
    write_to_file(filename,1);
    sleep(1);
    write_to_file(filename,1);
    sleep(1);
    write_to_file(filename,1);
    sleep(1);
    write_to_file(filename,1);
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
    sleep(1);
    write_to_file(file_5,0);
    // sleep(8);
    write_to_file(file_2,1);
    write_to_file(filename2,0);
    // sleep(2);
    write_to_file(file_6,1);
    write_to_file(filename,1);
    // sleep(2);
    write_to_file(filename,1);
    // sleep(2);
    write_to_file(filename,0);
    // sleep(2);
    write_to_file(filename,0);
    // sleep(2);
    write_to_file(filename,1);
    // sleep(2);
    write_to_file(filename,1);
    // int x = extract_tree_to_json();


    end();

    return 0;
}
