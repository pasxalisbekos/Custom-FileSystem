
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
    // printf("====================================================================================================\n");
    // printf("Read from file: %s\n", buffer);
    // printf("====================================================================================================\n");

    close(fd);

}

void custom_rw(){
    
    
    handler_init();

    char *filename = "./dir_a/dir_b/dir_c/example.txt";
    char *filename2 = "./dir_a/dir_b/dir_c/example2.txt";
    char *file_1 = "/home/pbekos/pass.txt";
    char *file_2 = "/home/pbekos/fail.txt";
    char *file_3 = "/home/pbekos/Documents/hw2-master/hw2/dtmf_in.txt";
    char *file_4 = "/home/pbekos/Desktop/100_websites_no_consent_fbpixel.txt";
    char *file_5 = "/home/pbekos/Desktop/rerun/grader-s19/requirements.txt";
    char *file_6 = "/home/pbekos/Desktop/test12/test.txt";


    // With delay between operations
    write_to_file(filename,1);
    sleep(1);
    read_from_file(file_2);
    sleep(2);
    write_to_file(filename,1);
    sleep(1);
    write_to_file(filename,1);
    sleep(1);
    write_to_file(filename,1);
    sleep(2);
    read_from_file(file_6);
    sleep(1);
    write_to_file(filename,1);
    sleep(1);
    read_from_file(file_2);
    sleep(2);
    read_from_file(file_3);
    sleep(2);
    write_to_file(filename,1);
    sleep(1);
    write_to_file(filename,1);
    sleep(1);
    write_to_file(filename,1);
    sleep(1);
    read_from_file(filename);
    sleep(2);
    write_to_file(filename,1);
    sleep(2);
    write_to_file(file_1,1);
    sleep(5);
    write_to_file(file_2,0);
    sleep(2);
    read_from_file(filename);
    sleep(3);
    write_to_file(file_5,1);
    sleep(10);
    read_from_file(file_1);
    sleep(2);
    read_from_file(filename2);
    sleep(2);
    read_from_file(filename2);
    sleep(2);
    write_to_file(file_3,1);
    sleep(2);
    read_from_file(filename);
    sleep(2);
    write_to_file(filename2,1);
    sleep(9);
    read_from_file(filename);
    write_to_file(file_4,0);
    sleep(1);
    write_to_file(file_5,0);
    read_from_file(file_1);
    sleep(8);
    write_to_file(file_2,1);
    sleep(2);
    read_from_file(filename);
    sleep(2);
    read_from_file(filename2);
    sleep(2);
    read_from_file(filename2);
    sleep(2);
    write_to_file(file_6,1);
    sleep(2);
    write_to_file(filename,1);
    sleep(2);
    read_from_file(filename);
    sleep(2);
    read_from_file(file_2);
    sleep(2);
    write_to_file(filename,1);
    sleep(2);
    read_from_file(file_1);
    sleep(2);
    write_to_file(file_1,0);
    sleep(5);
    write_to_file(file_2,1);
    sleep(2);
    read_from_file(filename);
    sleep(3);
    write_to_file(file_5,1);
    sleep(2);
    read_from_file(filename);
    sleep(10);
    write_to_file(file_3,0);
    sleep(9);
    read_from_file(filename);
    sleep(2);
    write_to_file(file_4,1);
    sleep(1);
    write_to_file(file_5,0);
    sleep(2);
    read_from_file(file_2);
    sleep(8);
    write_to_file(file_2,1);
    sleep(2);
    write_to_file(filename2,0);
    sleep(2);
    read_from_file(filename);
    sleep(2);
    write_to_file(file_6,1);
    sleep(2);
    write_to_file(filename,1);
    sleep(2);
    write_to_file(filename,1);
    sleep(2);
    read_from_file(filename);
    sleep(2);
    write_to_file(filename,0);
    sleep(2);
    read_from_file(filename);
    sleep(2);
    read_from_file(file_2);
    sleep(2);
    write_to_file(file_1,0);
    sleep(2);
    write_to_file(file_1,1);
    sleep(2);
    read_from_file(file_1);
    sleep(2);
    write_to_file(file_1,1);
    sleep(2);
    read_from_file(filename);

    // No delay
    /*
    write_to_file(filename,1);
    read_from_file(file_2);
    write_to_file(filename,1);
    write_to_file(filename,1);
    write_to_file(filename,1);
    read_from_file(file_6);
    write_to_file(filename,1);
    read_from_file(file_2);
    read_from_file(file_3);
    write_to_file(filename,1);
    write_to_file(filename,1);
    write_to_file(filename,1);
    read_from_file(filename);
    write_to_file(filename,1);
    write_to_file(file_1,1);
    write_to_file(file_2,0);
    read_from_file(filename);
    write_to_file(file_5,1);
    read_from_file(file_1);
    read_from_file(filename2);
    read_from_file(filename2);
    write_to_file(file_3,1);
    read_from_file(filename);
    write_to_file(filename2,1);
    read_from_file(filename);
    write_to_file(file_4,0);
    write_to_file(file_5,0);
    read_from_file(file_1);
    write_to_file(file_2,1);
    read_from_file(filename);
    read_from_file(filename2);
    read_from_file(filename2);
    write_to_file(file_6,1);
    write_to_file(filename,1);
    read_from_file(filename);
    read_from_file(file_2);
    write_to_file(filename,1);
    read_from_file(file_1);
    write_to_file(file_1,0);
    write_to_file(file_2,1);
    read_from_file(filename);
    write_to_file(file_5,1);
    read_from_file(filename);
    write_to_file(file_3,0);
    read_from_file(filename);
    write_to_file(file_4,1);
    write_to_file(file_5,0);
    read_from_file(file_2);
    write_to_file(file_2,1);
    write_to_file(filename2,0);
    read_from_file(filename);
    write_to_file(file_6,1);
    write_to_file(filename,1);
    write_to_file(filename,1);
    read_from_file(filename);
    write_to_file(filename,0);
    read_from_file(filename);
    read_from_file(file_2);
    write_to_file(file_1,0);
    write_to_file(file_1,1);
    read_from_file(file_1);
    write_to_file(file_1,1);
    read_from_file(filename);
    */
    
    end();
}


void read_from_file_default(char* filename){
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file for reading");
        return 1;
    }

    char buffer[100];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
    if (bytes_read == -1) {
        perror("Error reading from file");
        close(fd);
        return 1;
    }
    // printf("====================================================================================================\n");
    // printf("Read from file: %s\n", buffer);
    // printf("====================================================================================================\n");

    printf("READ FROM %s\n",filename);
    close(fd);
}

void write_to_file_default(char* filename, int flag){
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
    ssize_t bytes_written = write(fd, str, strlen(str));
    if (bytes_written == -1) {
        perror("Error writing to file");
    }

    printf("WRITE IN %s\n",filename);
    close(fd);
}



void default_rw(){
    
    char *filename = "./dir_a/dir_b/dir_c/example.txt";
    char *filename2 = "./dir_a/dir_b/dir_c/example2.txt";
    char *file_1 = "/home/pbekos/pass.txt";
    char *file_2 = "/home/pbekos/fail.txt";
    char *file_3 = "/home/pbekos/Documents/hw2-master/hw2/dtmf_in.txt";
    char *file_4 = "/home/pbekos/Desktop/100_websites_no_consent_fbpixel.txt";
    char *file_5 = "/home/pbekos/Desktop/rerun/grader-s19/requirements.txt";
    char *file_6 = "/home/pbekos/Desktop/test12/test.txt";


    write_to_file_default(filename,1);
    sleep(1);
    read_from_file_default(file_2);
    sleep(2);
    write_to_file_default(filename,1);
    sleep(1);
    write_to_file_default(filename,1);
    sleep(1);
    write_to_file_default(filename,1);
    sleep(2);
    read_from_file_default(file_6);
    sleep(1);
    write_to_file_default(filename,1);
    sleep(1);
    read_from_file_default(file_2);
    sleep(2);
    read_from_file_default(file_3);
    sleep(2);
    write_to_file_default(filename,1);
    sleep(1);
    write_to_file_default(filename,1);
    sleep(1);
    write_to_file_default(filename,1);
    sleep(1);
    read_from_file_default(filename);
    sleep(2);
    write_to_file_default(filename,1);
    sleep(2);
    write_to_file_default(file_1,1);
    sleep(5);
    write_to_file_default(file_2,0);
    sleep(2);
    read_from_file_default(filename);
    sleep(3);
    write_to_file_default(file_5,1);
    sleep(10);
    read_from_file_default(file_1);
    sleep(2);
    read_from_file_default(filename2);
    sleep(2);
    read_from_file_default(filename2);
    sleep(2);
    write_to_file_default(file_3,1);
    sleep(2);
    read_from_file_default(filename);
    sleep(2);
    write_to_file_default(filename2,1);
    sleep(9);
    read_from_file_default(filename);
    write_to_file_default(file_4,0);
    sleep(1);
    write_to_file_default(file_5,0);
    read_from_file_default(file_1);
    sleep(8);
    write_to_file_default(file_2,1);
    sleep(2);
    read_from_file_default(filename);
    sleep(2);
    read_from_file_default(filename2);
    sleep(2);
    read_from_file_default(filename2);
    sleep(2);
    write_to_file_default(file_6,1);
    sleep(2);
    write_to_file_default(filename,1);
    sleep(2);
    read_from_file_default(filename);
    sleep(2);
    read_from_file_default(file_2);
    sleep(2);
    write_to_file_default(filename,1);
    sleep(2);
    read_from_file_default(file_1);
    sleep(2);
    write_to_file_default(file_1,0);
    sleep(5);
    write_to_file_default(file_2,1);
    sleep(2);
    read_from_file_default(filename);
    sleep(3);
    write_to_file_default(file_5,1);
    sleep(2);
    read_from_file_default(filename);
    sleep(10);
    write_to_file_default(file_3,0);
    sleep(9);
    read_from_file_default(filename);
    sleep(2);
    write_to_file_default(file_4,1);
    sleep(1);
    write_to_file_default(file_5,0);
    sleep(2);
    read_from_file_default(file_2);
    sleep(8);
    write_to_file_default(file_2,1);
    sleep(2);
    write_to_file_default(filename2,0);
    sleep(2);
    read_from_file_default(filename);
    sleep(2);
    write_to_file_default(file_6,1);
    sleep(2);
    write_to_file_default(filename,1);
    sleep(2);
    write_to_file_default(filename,1);
    sleep(2);
    read_from_file_default(filename);
    sleep(2);
    write_to_file_default(filename,0);
    sleep(2);
    read_from_file_default(filename);
    sleep(2);
    read_from_file_default(file_2);
    sleep(2);
    write_to_file_default(file_1,0);
    sleep(2);
    write_to_file_default(file_1,1);
    sleep(2);
    read_from_file_default(file_1);
    sleep(2);
    write_to_file_default(file_1,1);
    sleep(2);
    read_from_file_default(filename);

}



int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Usage: %s <value>\n", argv[0]);
        return 1;
    }

    int mode = atoi(argv[1]);

    if (mode == 0) {
        printf("Executing default read - write - append operations\n");
        default_rw();
    } else if (mode == 1) {
        printf("Executing custom read - write - append operations\n");
        custom_rw();
    } else {
        printf("Unknown mode %d\n",mode);
    }





    return 0;
}


