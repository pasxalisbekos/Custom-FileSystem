
#include "custom_filesys.h"
#include "cleanup_daemon.h"
#include "log.h"
#include "operational.h"

void write_to_file(char* filename){
    
    int fd = open(filename, O_CREAT | O_WRONLY, 0777);
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

    
    // char** lines = read_operations("./operations.txt");
    // print_operations(lines);
    // monitor_main("./operations.txt");
    // return 0;


    init_spinlock();

    char *filename = "./dir_a/dir_b/dir_c/example.txt";
    char *filename2 = "./dir_a/dir_b/dir_c/example2.txt";
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

    // A series of write operations on different files to verify the directory tree creation, the snapshot preservation 
    // and the module that saves the structures into a JSON file so we can later (after a system crash) reconstruct the tree
    write_to_file(file_1);
    // sleep(5);
    write_to_file(file_2);
    // sleep(3);
    write_to_file(file_5);
    // sleep(10);
    write_to_file(file_3);
    write_to_file(filename2);
    // sleep(9);
    write_to_file(file_4);
    // sleep(1);
    write_to_file(file_5);
    // sleep(8);
    write_to_file(file_2);
    // sleep(2);
    write_to_file(file_6);
    write_to_file(filename2);
    write_to_file(file_1);
    // sleep(5);
    write_to_file(file_2);
    // sleep(3);
    write_to_file(file_5);
    // sleep(10);
    write_to_file(file_3);
    // sleep(9);
    write_to_file(file_4);
    // sleep(1);
    write_to_file(file_5);
    // sleep(8);
    write_to_file(file_2);
    write_to_file(filename2);
    // sleep(2);
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


    // list_sub_directories("/home/snapshots");

    destroy_spinlock();
    // print_tree_recursive(directory_tree_head,0);

    cJSON* json_tree = dir_tree_to_json(directory_tree_head);

    // Write JSON to file
    write_json_to_file(json_tree, "/home/snapshots/directory_tree.json");

    // Free cJSON object
    cJSON_Delete(json_tree);

    // directory_tree_head = NULL;
    // // Construct the directory tree
    // printf("=========================================================================\n");
    // dir_node* temp = construct_tree("directory_tree.json",getpid());
    // printf("=========================================================================\n");
   
    
    // print_tree_recursive(directory_tree_head, 0);

    // cJSON* json_tree2 = dir_tree_to_json(directory_tree_head);

    // // Write JSON to file
    // write_json_to_file(json_tree2, "directory_tree2.json");
    return 0;
}
