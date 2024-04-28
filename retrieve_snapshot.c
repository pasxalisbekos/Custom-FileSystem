#include "custom_filesys.h"
#include "cleanup_daemon.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, char* argv[]){

    if(argc != 2){
        printf("Invalid arguments. Execution: ./snap_ret <path_to_file>\n");
        return 0;
    }
    else{
        char* input_path = strdup(argv[1]);
        char* abs_path = realpath(input_path,NULL);
        if(abs_path == NULL){
            printf("Cannot find file: %s\n",abs_path);
            return 0;
        }


        printf("===================================================================================================================================================================================\n");
        printf("All snapshots for {%s}\n",abs_path);
        get_all_snapshots_for_file(abs_path);
        char* most_recent_snapshot = get_most_recent_snapshot_for_file(abs_path);
        printf("-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        printf("Most recent snapshot: [%s]\n",most_recent_snapshot);
        printf("-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        printf("===================================================================================================================================================================================\n");
        

    
        char input;
        printf("Revert to previous snapshot? (y/n): ");
        scanf(" %c", &input); // Note the space before %c to consume any leading whitespace
        // Flush remaining characters in input buffer
        while (getchar() != '\n');

        while (input != 'y' && input != 'n') {
            printf("Invalid input. Please enter 'y' for yes or 'n' for no: ");
            scanf(" %c", &input);
            
            // Flush remaining characters in input buffer
            while (getchar() != '\n');
        }

        if (input == 'y'){
            printf("Enter the snapshot name (not absolute path):");
            char* input;
            size_t len = 0;
            ssize_t bytes_read;

            bytes_read = getline(&input, &len, stdin);
            if (bytes_read == -1) {
                printf("Error reading input.\n");
                return 1;
            }
            if (input[bytes_read - 1] == '\n') {
                input[bytes_read - 1] = '\0'; // Replace newline with null terminator
            }
            // Print the input
            // printf("You entered: %s", input);
            char* specific_snap = get_snapshot("/home/snapshots/",input);
            if(specific_snap == NULL){
                printf("Invalid snapshot name {%s}\n",input);
            }else{
                printf("Reverting back to snapshot: {%s}\n",specific_snap);
                int revert_flag = revert_to_previous_version(abs_path,specific_snap);
                if (revert_flag == 0){
                    printf(" > Rollback successfull\n * Exiting gracefully\n");
                }else{
                    printf("Rollback failed with status {%d}\n",revert_flag);
                }
            }
        }        
    }

    return 0;
}