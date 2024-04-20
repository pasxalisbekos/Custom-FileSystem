#include <stdio.h>
#include <string.h>
#include <time.h>
#include "operational.h"


int main(int argc, char *argv[]) {
    
    // random crash
    srand(time(NULL));


    // FIRST THING TO DO IS SET UP THE SIGNAL HANDLER TO HANDLE CRASHES
    handler_init();

    // Initiallize the current path:
    

    // Check if the correct number of arguments are provided
    if (argc < 2 || argc > 4) {
        printf("Usage: %s operation [source_path] [destination_path]\n", argv[0]);
        return 1;
    }

    // Extract operation, source, and destination from command-line arguments
    char *operation = argv[1];
    char *source = (argc >= 3) ? argv[2] : NULL;
    char *destination = (argc == 4) ? argv[3] : NULL;

    // Check if operation is WRITE 
    if (strcmp(operation, "WRITE") == 0 && source != NULL && destination != NULL) {
        update_curr_path(destination);
        printf("Operation: %s\nSource: %s\nDestination: %s\n", operation, source, destination);
        copy_file(source,destination);
    }
    // Check if operation is READ 
    else if (strcmp(operation, "READ") == 0 && source != NULL && destination == NULL) {
        update_curr_path(source);
        printf("Operation: %s\nSource: %s\n", operation, source);
    }
    // Invalid combination of arguments
    else {
        printf("Invalid combination of arguments\n");
        return 1;
    }

    int randomNum = rand() % 10 + 1;
    if (randomNum == 5) {
            int* ptr = NULL;
            *ptr = 42;
    }

    // Verified that the handlers work
    // int* ptr = NULL;
    // *ptr = 42; // This will cause a segmentation fault
    return 0;
}
