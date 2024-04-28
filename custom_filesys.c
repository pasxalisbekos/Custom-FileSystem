#include "custom_filesys.h"

dir_node* directory_tree_head = NULL;
char* current_file_path_operated_on = NULL;


pthread_spinlock_t json_tree_lock;
int is_tree_json_lock_initialized = 0;


char* real_path(char* filepath){
    return realpath(filepath,NULL);
}

void end(){

    // directory_tree_head = NULL;
    // int line_count = 0;
    // char** prev_paths = read_absolute_paths(&line_count);
    
    // if(prev_paths != NULL){
    //     // printf("================================================================================================\n");
    //     // printf("%d\n",line_count);
    //     for(int i=0; i < line_count; i++){
    //         // printf("----------> %s\n",prev_paths[i]);
    //         char** path_split = str_split(prev_paths[i],'|');
            
    //         // Get the path to the file from the absolute_paths.txt
    //         char* path = realpath(path_split[0],NULL);
    //         // Get the corresponding operation performed on that file
    //         int operation = atoi(path_split[1]);
    //         // printf("FOUND PREVIOUS OPERATION: %d ON FILE: %s\n",operation,path);
    //         add_directory_to_tree(path,getpid(), 0, operation);
    //     }
    //     // printf("================================================================================================\n");

    // } 
    tree_write();
}

/**
 * @brief This is the global signal handler that each thread using our custom write/read should first initiallize it. Upon a crash
 * we must be able to preserve the current status of the system (write the tree directory to the JSON file)
 *
 * @param signum : The signal number (e.g. seg fault: 11)
 */
 void custom_signal_handler(int signum) {
    
    if(current_file_path_operated_on == NULL){
        printf("No file was operated when receiving signal {%d}, exiting...\n",signum);
        exit(signum);
    }
    
    printf("Caught signal [ %d ]| Operation path: {%s}\n", signum, current_file_path_operated_on);
    
    char* most_recent_snap = get_most_recent_snapshot_for_file(current_file_path_operated_on);
    printf("Most recent snapshot path : [   %s  ]\n",most_recent_snap);


    // Ask for user input on what to do with the file
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
        int revert_flag = revert_to_previous_version(current_file_path_operated_on,most_recent_snap);
        if (revert_flag == 0){
            printf(" > Rollback successfull\n * Exiting gracefully\n");
            // store the json and any other file remaining
            // extract_tree_to_json();
        }else{
            printf("Rollback failed with status {%d}\n",revert_flag);
        }
    }
    
    extract_tree_to_json();
    exit(signum);
}

/**
 * @brief This is the initializer function for our signal handler. It sets up our handler for the following signals
 *  1) SIGKILL : Process killed
 *  2) SIGSEGV : Segmentation fault
 *  3) SIGABRT : Abort signal
 *  4) SIGILL  : Illegal instruction
 *  5) SIGFPE  : Floating point exception
 *  6) SIGTERM : Request to the program to terminate 
 *  7) SIGINT  : Generated when using CTRL+C
 *  Those are some initial signals that our implementation is handling, any addition for a new signal should be written inside
 *  this function.
 */
void handler_init(){
    // Setting up our signal handler for all those signals
    signal(SIGKILL, custom_signal_handler); 
    signal(SIGSEGV, custom_signal_handler); 
    signal(SIGABRT, custom_signal_handler); 
    signal(SIGILL,  custom_signal_handler); 
    signal(SIGFPE,  custom_signal_handler);  
    signal(SIGTERM,  custom_signal_handler); 
    signal(SIGINT,  custom_signal_handler); 
}




// --------------------------------------------------------------------- HELPER FUNCTIONS --------------------------------------------------------------------- //
/**
 * @brief Get the permissions of a file as a string representation
 * 
 * @param path_to_file : The absolute path to a file
 * @return char*  : The string representation of the permissions
 */
char *get_permissions(char *path_to_file) {
    struct stat file_stat;

    if (stat(path_to_file, &file_stat) == -1) {
        perror("Error getting file status");
        return NULL;
    }

    char *permissions = (char *)malloc(10 * sizeof(char));
    if (permissions == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }

    // Fill permissions string based on file mode
    sprintf(permissions, "%c%c%c%c%c%c%c%c%c",
            (S_ISDIR(file_stat.st_mode)) ? 'd' : '-',
            (file_stat.st_mode & S_IRUSR) ? 'r' : '-',
            (file_stat.st_mode & S_IWUSR) ? 'w' : '-',
            (file_stat.st_mode & S_IXUSR) ? 'x' : '-',
            (file_stat.st_mode & S_IRGRP) ? 'r' : '-',
            (file_stat.st_mode & S_IWGRP) ? 'w' : '-',
            (file_stat.st_mode & S_IXGRP) ? 'x' : '-',
            (file_stat.st_mode & S_IROTH) ? 'r' : '-',
            (file_stat.st_mode & S_IWOTH) ? 'w' : '-',
            (file_stat.st_mode & S_IXOTH) ? 'x' : '-');

    return permissions;
}

/**
 * @brief Get the file size 
 * 
 * @param filename 
 * @return long 
 */
long get_file_size(char *path_to_file) {
    FILE *file = fopen(path_to_file, "rb"); 
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        perror("Error seeking to end of file");
        fclose(file);
        return -1;
    }

    // Get the current position in the file, which corresponds to the file size
    long size = ftell(file);
    if (size == -1) {
        perror("Error getting file size");
        fclose(file);
        return -1;
    }

    fclose(file);
    return size;
}
/**
 * @brief Get the file ownership of a file
 * 
 * @param filename : The absolute path to a file
 * @return char* : A string representation of the ownership of that file
 */
char *get_file_ownership(char *path_to_file) {
    struct stat file_stat;
    struct passwd *owner_info;
    struct group *group_info;

    // Get file status
    if (stat(path_to_file, &file_stat) == -1) {
        perror("Error getting file status");
        return NULL;
    }

    // Get owner information
    owner_info = getpwuid(file_stat.st_uid);
    if (owner_info == NULL) {
        perror("Error getting owner information");
        return NULL;
    }

    // Get group information
    group_info = getgrgid(file_stat.st_gid);
    if (group_info == NULL) {
        perror("Error getting group information");
        return NULL;
    }

    // Allocate memory for ownership string
    size_t ownership_size = strlen(owner_info->pw_name) + strlen(group_info->gr_name) + 2; // +2 for ':' and '\0'
    char *ownership = (char *)malloc(ownership_size * sizeof(char));
    if (ownership == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }

    // Format ownership string
    snprintf(ownership, ownership_size, "%s:%s", owner_info->pw_name, group_info->gr_name);

    return ownership;
}

/**
 * @brief  This function takes as argument a string and returns its sha256 hashed representation
 * 
 * @param input : The input string
 * @return char* : The sha256 output
 */
char *sha256(const char *input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    char *output = (char *)malloc(2 * SHA256_DIGEST_LENGTH + 1); // +1 for null terminator
    
    if (output == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input, strlen(input));
    SHA256_Final(hash, &sha256);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output + 2 * i, "%02x", hash[i]);
    }

    output[2 * SHA256_DIGEST_LENGTH] = '\0'; // Null-terminate the string

    return output;
}
/**
 * @brief This function is a custom split, that splits the string file_path based on the delimiter char
 * and returns an array of strings representing its tokens
 * @param file_path : The string we want to split
 * @param delimeter : The char that we split the string according to
 * @return char**   : The sub-strings after the split
 */
char** str_split(char* file_path, const char delimeter) {
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = file_path;
    char* last_token = 0;
    char delim[2];
    delim[0] = delimeter;
    delim[1] = 0;

    char new_file_path[strlen(file_path)+1];
    strcpy(new_file_path,file_path);


    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (delimeter == *tmp)
        {
            count++;
            last_token = tmp;
        }
        tmp++;
    }
    if(count == 0){
        return NULL; // e.g. "file.txt"
    }else{
        count += last_token < (file_path + strlen(file_path) - 1);
        result = malloc(sizeof(char*) * count);

        if (result)
        {
            size_t idx  = 0;

            char* token = strtok(new_file_path, delim);

            while (token)
            {
                assert(idx < count);
                *(result + idx++) = strdup(token);
                token = strtok(0, delim);
            }
            *(result + idx) = 0;
        }

        return result;
    }

}
/**
 * @brief This function takes as arguments two strings start and end and combines them into one (start+end)
 * 
 * @param start : The starting string
 * @param add   : The remainder string
 * @return char* : The result of combination of the above strings
 */
char *append_strings(char *start, char *add) {
  char *result = malloc(strlen(start) + strlen(add) + 1);
  
  if (!result) {
    return 0;
  } 
  else {
    char *copy = result;
    while (*start != '\0')
        *copy++ = *start++;
    while (*add != '\0')
        *copy++ = *add++;
    *copy = 0;
  }

  return result;
}

/**
 * @brief Get the last element of an array of strings
 * 
 * @param array : The array of strings
 * @return char* : The last string
 */
char* get_last_element(char** array) {
    char* last_element = NULL;
    for (int i = 0; array[i] != NULL; i++) {
        last_element = array[i];
    }
    return last_element;
}

/**
 * @brief Returns the current timestamp in a human readble forman e.g. 2024-04-16_15:00:26
 * 
 * @return char* : The string representation of the timestamp
 */
char* current_timestamp_as_string() {
    time_t current_time;
    struct tm* time_info;
    static char timestamp[MAX_TIMESTAMP_LENGTH];

    time(&current_time);
    time_info = localtime(&current_time);

    // Format the time as a string
    strftime(timestamp, MAX_TIMESTAMP_LENGTH, "%Y-%m-%d_%H:%M:%S", time_info);

    return timestamp;

    // Timestamp in ms depth

    // struct timeval tv;
    // struct tm* time_info;
    // static char timestamp[MAX_TIMESTAMP_LENGTH];

    // gettimeofday(&tv, NULL);
    // time_info = localtime(&tv.tv_sec);

    // // Format the time as a string
    // snprintf(timestamp, MAX_TIMESTAMP_LENGTH, "%04d-%02d-%02d_%02d:%02d:%02d:%03ld",
    //          time_info->tm_year + 1900, time_info->tm_mon + 1, time_info->tm_mday,
    //          time_info->tm_hour, time_info->tm_min, time_info->tm_sec,
    //          tv.tv_usec / 1000);

    // return timestamp;
}

/**
 * @brief This function replaces all occurences of the "find" char inside the "str" string with the 
 * "replace" string
 * @param str : The string we are modifying
 * @param find : The char we are looking for
 * @param replace : The char that we replace the "find" char with
 * @return char* : The modified version of the string
 */
char* replace_char(char* str, char find, char replace){
    char *current_pos = strchr(str,find);
    while (current_pos) {
        *current_pos = replace;
        current_pos = strchr(current_pos,find);
    }
    return str;
}
// --------------------------------------------------------------------- FILE LIST FUNCTIONS --------------------------------------------------------------------- //
int copy_file(char* src_path, char* dest_path) {
    FILE *src_file, *dest_file;
    char buffer[BUFSIZ];
    size_t n;

    // Open source file for reading
    src_file = fopen(src_path, "rb");
    if (src_file == NULL) {
        perror("Error opening source file");
        return -1;
    }

    // Open destination file for writing
    dest_file = fopen(dest_path, "wb");
    if (dest_file == NULL) {
        perror("Error creating destination file");
        fclose(src_file);
        return -1;
    }

    // Copy data from source to destination
    while ((n = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        if (fwrite(buffer, 1, n, dest_file) != n) {
            perror("Error writing to destination file");
            fclose(src_file);
            fclose(dest_file);
            return -1;
        }
    }

    // Close files
    fclose(src_file);
    fclose(dest_file);
    return 0;
}
/**
 * @brief This function is used to create a snapshot of a file.
 * 1) Let's assume that all snapshots are saved in the /home/snapshot directory (we need to initiallize its creation somewhere most likely in the first read or write operation)
 * 2) Now, every file should be stored to a sub-directory which has as name the hash of the path 
        a) Assume path/to/temp/file.txt is the path
        b) The name of directory where all snapshots of this file are stored is: /home/snapshots/hash(path/to/temp/file.txt)
        c) If there is no such directory -> create it
           Else store the snapshot in that directory
 * 3) Snapshot name is: timestamp_filename

 * @param file_name : The name of the file we are operating on
 * @param absolute_path : The absolute path to that file
 * @param timestamp  : The timestamp of the operation
 * @return char* : Returns that absolute path to the directory where that snapshot was stored
 */
char* create_snapshot(char* file_name, char* absolute_path, char* timestamp){
    //1)  Snapshot file name creation e.g. 2024-04-16 13:23:25_pass.txt
    char* temp_filename = strdup(append_strings("_",file_name));
    char* snapshot_file_name = strdup(append_strings(timestamp,temp_filename));
    // printf("%s\n",snapshot_file_name);
    //2) Snapshot directory creation/search
    struct stat st = {0};
    char* baseline_dir = strdup("/home/snapshots/");
    char* hashed_dir_name = strdup(sha256(absolute_path));
    char* snapshot_dir_name = append_strings(baseline_dir,hashed_dir_name);

    // printf("%s,\n", absolute_path);
    // If the snapshot directory does not exist create it
    if (stat(snapshot_dir_name, &st) == -1) {
        int temp = mkdir(snapshot_dir_name, 0777);
        if (temp != 0){
            printf("Error creating snapshot directory\n");
            return NULL;
        }
    }   
    

    // 3) Create the absolute path to the snapshot e.g. --> /home/snapshots/9b24f0a105528cc2fab1c1324299ebcf965dd35cc83114623604dd214175eb29/2024-04-16 13:42:26_pass.txt
    char* temp_snap_dir = append_strings(snapshot_dir_name,"/");
    char* snapshot_absolute_path = append_strings(temp_snap_dir,snapshot_file_name);

    // 4) Now create a snapshot of the original file
    char command[1024];
    //                                              source_path       destination_path
    snprintf(command, sizeof(command), "cp %s %s", absolute_path, snapshot_absolute_path);

    int status = system(command);
    if (status == -1) {
        perror("Error executing system command");
        return NULL;
    } else if (status != 0) {
        fprintf(stderr, "Command failed with exit status %d\n", status);
        return NULL;
    }else{

    }

    return snapshot_absolute_path;
}

/**
 * @brief Searches for a file in the files list that corresponds to the name
 * 
 * @param head : A pointer to the head of the files list
 * @param file_name : The name of the file we are searching for
 * @return file_node* : NULL if file not found else the pointer to the file
 */
file_node* search_for_file_node(file_node* head, char* file_name){

    file_node* curr = head;
    while(curr != NULL){
        if (strcmp(curr->file_name,file_name) == 0){
            // printf("File name %s exists\n",file_name);
            return curr;
        }
        curr = curr->next;
    }

    return NULL;
}

/**
 * @brief This function is used when we are operating on a file that already exists in our tree. If the file is already stored for a directory node
 * we only need to preserve the snapshot

 * @param head : A pointer to the head of the files list
 * @param file_name : The name of the file
 * @param absolute_path  : The absolute path to the file
 */
void update_only_snapshot(snapshot** head, char* file_name, char* absolute_path, int PID, int operation_flag){

    snapshot* new_snapshot = (snapshot*)malloc(sizeof(snapshot));
    if (new_snapshot == NULL){
        return;
    }
    
    
    char* curr_timestamp = current_timestamp_as_string();
    char* snapshot_dir = create_snapshot(file_name,absolute_path,curr_timestamp);
    new_snapshot->snapshot_path = strdup(snapshot_dir);
    new_snapshot->timestamp = strdup(curr_timestamp);
    new_snapshot->PID = PID;
    new_snapshot->next = *head;
    *head = new_snapshot;

}

/**
 * @brief This function "pushes" a new file node to the list of files for a specific directory node
 * 
 * @param head_ref : A pointer to the head of the files list for the current directory node
 * @param file_name : The name of the file
 * @param absolute_path : The absolute path to the files
 */
void push(file_node** head_ref, char* file_name, char* absolute_path, int PID, int flag, int operation_flag) {
    file_node* new_file = (file_node*)malloc(sizeof(file_node));
    if (new_file == NULL) {
        return;
    }
    
    new_file->file_name = strdup(file_name);
    if (new_file->file_name == NULL) {
        free(new_file);
        return;
    }

    char* curr_timestamp = current_timestamp_as_string();

    new_file->file_hash = strdup(sha256(file_name));
    new_file->absolute_path = strdup(absolute_path);
    new_file->versions_path = NULL;
    new_file->operations_size = 1;
    new_file->operations = (char*)malloc(new_file->operations_size * sizeof(char));
    new_file->permissions = strdup(get_permissions(absolute_path));
    new_file->size = get_file_size(absolute_path);
    new_file->ownership = strdup(get_file_ownership(absolute_path));

    // printf("%d\n",operation_flag);
    switch(operation_flag){
        case 0:
            new_file->operations[0] = 'R';
            break;
        case 1:
            new_file->operations[0] = 'W';
            break;
        case 2:
            new_file->operations[0] = 'A';
            break;
        default:
            new_file->operations[0] = 'U';
            break;
    }



    // char command[1024];
    // snprintf(command, sizeof(command), "cat '%s'", absolute_path);

    // // Execute the command
    // int result = system(command);
    // if (result == -1) {
    //     perror("Error executing command");
    //     return -1;
    // } else if (result != 0) {
    //     printf("Failed to execute command.\n");
    //     return -1;
    // }
    char* snapshot_dir = create_snapshot(file_name,absolute_path,curr_timestamp);
    // printf("%s\n",snapshot_dir);
    new_file->snapshot = (snapshot*)malloc(sizeof(snapshot));
    new_file->snapshot->snapshot_path = strdup(snapshot_dir);
    new_file->snapshot->timestamp = strdup(curr_timestamp);
    new_file->snapshot->PID = PID;
    new_file->snapshot->next = NULL;

    if (new_file->operations[0] != 'R'){
        // Initially just store the path to the snapshots and the timestamp
        char* snapshot_dir = create_snapshot(file_name,absolute_path,curr_timestamp);
        // printf("%s\n",snapshot_dir);
        new_file->snapshot = (snapshot*)malloc(sizeof(snapshot));
        new_file->snapshot->snapshot_path = strdup(snapshot_dir);
        new_file->snapshot->timestamp = strdup(curr_timestamp);
        new_file->snapshot->PID = PID;
        new_file->snapshot->next = NULL;

    }else{
        char* baseline_dir = strdup("/home/snapshots/");
        char* hashed_dir_name = strdup(sha256(absolute_path));
        char* snapshot_dir = append_strings(baseline_dir,hashed_dir_name);

        new_file->snapshot = (snapshot*)malloc(sizeof(snapshot));
        new_file->snapshot->snapshot_path = strdup(snapshot_dir);
        new_file->snapshot->timestamp = strdup(curr_timestamp);
        new_file->snapshot->PID = PID;
        new_file->snapshot->next = NULL;
    }

    new_file->next = *head_ref;
    
    *head_ref = new_file;
    // printf("%c\n",(*head_ref)->operations[(*head_ref)->operations_size]);
}


/**
 * @brief This function is just for verification purposes, prints the list of files for each directory node and also the snapshots that were stored for each file under that directory
 * 
 * @param head : A pointer to the head of the files list
 */
void print_list(file_node* head){
    
    file_node* curr = head;

    printf("\t\t\t[\n");
    while (curr != NULL)
    {
        printf("%s : {",curr->file_name);
        snapshot* curr_snap = curr->snapshot;
        if (curr_snap != NULL){
            while(curr_snap != NULL){
                printf("%s | %s , ",curr_snap->snapshot_path,curr_snap->timestamp);
                curr_snap = curr_snap->next;
            }
        }
        curr = curr->next;
        if(curr != NULL){
            printf("},\n");
        }
    }
    printf("]\n");
}

// --------------------------------------------------------------------- DIRECTORY FUNCTIONS --------------------------------------------------------------------- //
/**
 * @brief Create a dir node object based on the directory name
 * 
 * @param dir_name : The name of the directory
 * @return dir_node* : A dir_node object initialized
 */
dir_node* create_dir_node(const char* dir_name) {
    dir_node* new_node = (dir_node*)malloc(sizeof(dir_node));
    if (new_node == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    new_node->dir_name = strdup(dir_name);
    new_node->dir_hash = strdup(sha256(dir_name));
    new_node->children = NULL; // Initially, no children
    new_node->files_list = NULL; //Initially no files
    return new_node;
}

/**
 * @brief This is the core function that takes as argument the absolute path to a file, splits it to directory node which are stored in a hierarchical tree and appends to the
 * appropriate directory node the file

 * @param absolute_path : The absolute path to the file
 */
void add_directory_to_tree(char* absolute_path,int PID, int flag, int operation_flag) {

    // Tokenize the absolute path
    char* path_copy = strdup(absolute_path); // Duplicate the path to avoid modifying the original
    
    
    // Get the file name
    char* temp_path = strdup(absolute_path);
    char** relative_tokens = str_split(temp_path,'/');
    char* file = strdup(get_last_element(relative_tokens));
    // 
    
    char* token = strtok(path_copy, "/");
    if (token == NULL) {
        // Path is empty
        return;
    }

    // Check if the root node is empty
    if (directory_tree_head == NULL) {
        directory_tree_head = create_dir_node(token); // Initialize the root node
    }

    dir_node* current_node = directory_tree_head;

    // Traverse the path and add directories to the tree
    dir_node* last_directory;
    while (token != NULL) {
        if (strcmp(token,file) == 0){
            // Now we have to add the file to the current node's file's list
            last_directory = current_node;
            break;
        }
        // Search for the current directory node among the children
        dir_node* found_node = NULL;
        if (current_node->children != NULL) {
            for (int i = 0; current_node->children[i] != NULL; i++) {
                if (strcmp(current_node->children[i]->dir_name, token) == 0) {
                    found_node = current_node->children[i];
                    break;
                }
            }
        }
        
        if (found_node == NULL) {
            // No matching directory node found, create a new one
            dir_node* new_node = create_dir_node(token);
            add_child(current_node, new_node);
            current_node = new_node; // Move to the newly created node
        } else {
            // Matching directory node found, move to it
            current_node = found_node;
        }

        // Move to the next token
        token = strtok(NULL, "/");
        
    }


    // Here we must first search if this file already exists 
    // If not: push it to the directory node file list
    // Else: update its snapshots only
    file_node* temp = search_for_file_node(current_node->files_list, file);
    if (temp == NULL){
        // We found no file so add it to the list
        push(&(current_node->files_list), file , absolute_path, PID, flag, operation_flag);
        // printf("======================> %c\n",current_node->files_list->operations[0]);
    } else {
        // We must only update the snapshots for this file
        if(flag == 1){
            update_only_snapshot(&(temp->snapshot), file, absolute_path, PID, operation_flag);
        }
        
        size_t new_size = temp->operations_size + 1;
        char* new_operations = (char*)realloc(temp->operations, new_size * sizeof(char));
        if (new_operations == NULL) {
            fprintf(stderr, "Memory reallocation failed\n");
            exit(1); // or handle the error in another way
        }
        temp->operations = new_operations;
        switch(operation_flag){
            case 0:
                temp->operations[temp->operations_size] = 'R';
                break;
            case 1:
                temp->operations[temp->operations_size] = 'W';
                break;
            case 2:
                temp->operations[temp->operations_size] = 'A';
                break;
            default:
                temp->operations[temp->operations_size] = 'U';
                break;
        }
        
        temp->operations_size = new_size;
    
        
        // printf("%s\n",file);
    }
    free(path_copy); // Free the duplicated path
}


/**
 * @brief This function is a helper, used to add a directory node as child to its parent. For example assume the following directories:
 *      1) temp/dir_a
 *      2) temp/dir_b
 * On both cases the parent node is the temp directory and the children nodes are the dir_a/_b directories
 * @param parent : The parent directory node
 * @param child  : The child directory node
 */
void add_child(dir_node* parent, dir_node* child) {
    if (parent->children == NULL) {
        // If the parent has no children yet, allocate memory for the children array
        parent->children = (dir_node**)malloc(2 * sizeof(dir_node*)); // Allocate space for child and NULL terminator
        if (parent->children == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        parent->children[0] = child;
        parent->children[1] = NULL; // Null-terminate the array
    } else {
        // If the parent already has children, reallocate memory to accommodate the new child
        int num_children = 0;
        while (parent->children[num_children] != NULL) {
            num_children++;
        }
        parent->children = (dir_node**)realloc(parent->children, (num_children + 2) * sizeof(dir_node*)); // Allocate space for the new child and NULL terminator
        if (parent->children == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        parent->children[num_children] = child;
        parent->children[num_children + 1] = NULL; // Null-terminate the array
    }
}

/**
 * @brief A helper function that is used to print the directory tree and its contents recursively
 * 
 * @param node : The current node
 * @param depth : The current depth 
 */
void print_tree_recursive(dir_node* node, int depth) {
    if (node == NULL) {
        return;
    }
    // Print indentation based on depth
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    // Print directory name
    printf("|_ DIR: [%s]\n", node->dir_name,node->dir_hash);
    file_node* head = node->files_list;
    // Print files
    // if(head != NULL){
    //     print_list(head);
    // }
    // Recursively print children
    if (node->children != NULL) {
        int i = 0;
        while (node->children[i] != NULL) {
            print_tree_recursive(node->children[i], depth + 1);
            i++;
        }
    }
}




/**
 * @brief This function is used to transform a directory tree into a cJSON object
 * 
 * @param root : The root of the tree 
 * @return cJSON* 
 */
cJSON* dir_tree_to_json(dir_node* root) {
    cJSON* json_node = cJSON_CreateObject();
    cJSON* files_array = cJSON_CreateArray();
    cJSON* children_array = cJSON_CreateArray();
    
    // Add directory name and hash to JSON object
    cJSON_AddStringToObject(json_node, "dir_name", root->dir_name);
    cJSON_AddStringToObject(json_node, "dir_hash", root->dir_hash);
    file_node* file_ptr = root->files_list;
    while (file_ptr != NULL) {
        cJSON* file_json = cJSON_CreateObject();
        cJSON_AddStringToObject(file_json, "file_name", file_ptr->file_name);
        cJSON_AddStringToObject(file_json, "file_hash", file_ptr->file_hash);
        cJSON_AddStringToObject(file_json, "absolute_path", file_ptr->absolute_path);
        cJSON_AddStringToObject(file_json, "permissions", file_ptr->permissions);
        cJSON_AddStringToObject(file_json, "ownership", file_ptr->ownership );
        char size_str[100];
        snprintf(size_str, sizeof(size_str), "%ld", file_ptr->size);
        cJSON_AddStringToObject(file_json, "size", size_str);



        // Add file JSON object to files array
        cJSON_AddItemToArray(files_array, file_json);

        cJSON* operations_array = cJSON_CreateArray();
        size_t size = file_ptr->operations_size;
        for(int i = 0; i < size; i++) {
            // Convert each operation character to a string and add it to the operations array
            char operation_str[2] = {file_ptr->operations[i], '\0'}; // Create a string from the operation character
            cJSON_AddItemToArray(operations_array, cJSON_CreateString(operation_str));
        }
        
        // Add the operations array to the file JSON object
        cJSON_AddItemToObject(file_json, "operations", operations_array);
        
        cJSON* snapshots_array = cJSON_CreateArray();
        snapshot* head = file_ptr->snapshot;
        while(head != NULL){

            cJSON* snapshot_json = cJSON_CreateObject();
            cJSON_AddStringToObject(snapshot_json, "snapshot_path", head->snapshot_path);
            cJSON_AddStringToObject(snapshot_json, "snapshot_timestamp", head->timestamp);   
            cJSON_AddItemToArray(snapshots_array, snapshot_json);           

            head = head->next;
        }

        cJSON_AddItemToObject(file_json, "snapshots", snapshots_array);

        file_ptr = file_ptr->next;
    }

    cJSON_AddItemToObject(json_node, "files", files_array);

    // printf("%s\n",root->dir_name);
    if (root->children != NULL){
        
        for(int i = 0; root->children[i] != NULL; i++){
            cJSON* child_json = dir_tree_to_json(root->children[i]);
            cJSON_AddItemToArray(children_array, child_json);
        }    
    }

    cJSON_AddItemToObject(json_node, "children", children_array);

    return json_node;
}


/**
 * @brief This function is a helper that is used to write the cJSON represntation of our directory tree to a JSON file
 * 
 * @param json : The cJSON object
 * @param filename : The filename
 */
void write_json_to_file(cJSON* json, const char* filename) {
    
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }
    char* json_str = cJSON_Print(json);
    fprintf(fp, "%s", json_str);
    fclose(fp);
    free(json_str);
}


/**
 * @brief This function is used for reading a JSON file and reconstructing the tree accordingly. We do not re-construct the snapshot list
 * for each file because this is redundant. If a user wants to roll back to a previous version then they will use the snapshot directory
 * This will only be used once when the first "write" operation takes place that will try to reconstruct the tree and then add new nodes.
 *
 * @param file_path : The path to the JSON file
 * @param PID : The pid (process id) of the process performing the operation
 */
void construct_tree(char* file_path, int PID) {
    // printf("here\n");
    int count;
    // char* temp = strdup("directory_tree.json");
    char** paths = get_absolute_paths_from_json(file_path, &count);
    
    if (paths) {
        // printf("Absolute Paths:\n");
        for (int i = 0; i < count; i++) {
            // printf("%s\n", paths[i]);
            // free(paths[i]);
            add_directory_to_tree(paths[i],PID,0, -1);
        }
        free(paths); // Free memory allocated for paths array
    } else {
        printf("Failed to get absolute paths from JSON\n");
    }
    // printf("here\n");
    // print_tree_recursive(directory_tree_head, 0);
}

/**
 * @brief This is a helper function that collects all absolute paths to files stored in the JSON that reserves our directory tree
 * 
 * @param json_node : The cJSON object representing the contents of the file
 * @param paths : A array of strings that will include all absolute paths 
 * @param count : A variable to keep track of the number of absolut paths
 */
void collect_absolute_paths(cJSON* json_node, char*** paths, int* count) {
    // Extract files array
    cJSON* files_array = cJSON_GetObjectItem(json_node, "files");
    if (files_array) {
        int num_files = cJSON_GetArraySize(files_array);
        for (int i = 0; i < num_files; i++) {
            cJSON* file_item = cJSON_GetArrayItem(files_array, i);
            cJSON* absolute_path = cJSON_GetObjectItem(file_item, "absolute_path");
            if (absolute_path) {
                *paths = realloc(*paths, (*count + 1) * sizeof(char*));
                (*paths)[*count] = strdup(absolute_path->valuestring);
                (*count)++;
            }
        }
    }

    // Extract children directories
    cJSON* children_array = cJSON_GetObjectItem(json_node, "children");
    if (children_array) {
        int num_children = cJSON_GetArraySize(children_array);
        for (int i = 0; i < num_children; i++) {
            cJSON* child_item = cJSON_GetArrayItem(children_array, i);
            collect_absolute_paths(child_item, paths, count);
        }
    }
}


/**
 * @brief Get the absolute paths from json files
 * 
 * @param filename : The JSON file name where we are reading from
 * @param count : The number of absolute paths found
 * @return char** 
 */
char** get_absolute_paths_from_json(char* filename, int* count) {
    *count = 0;
    char** paths = NULL;

    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening JSON file\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* json_string = malloc(file_size + 1);
    if (!json_string) {
        printf("Error allocating memory\n");
        fclose(file);
        return NULL;
    }
    fread(json_string, 1, file_size, file);
    json_string[file_size] = '\0';

    // Parse JSON string
    cJSON* root = cJSON_Parse(json_string);
    free(json_string);
    fclose(file);

    if (!root) {
        printf("Error parsing JSON: %s| %s\n", cJSON_GetErrorPtr(),filename);
        return NULL;
    }

    collect_absolute_paths(root, &paths, count);

    // Cleanup
    cJSON_Delete(root);

    return paths;
}

/**
 * @brief 
 * 
 * @param PID 
 * @return int 
 */
int try_to_access_json(int PID){
    
    
    int fd = open("/home/snapshots/directory_tree.json", O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        return -1;
    }

    int spin_attempts = 0;
    int lock_acquired = 0;
    
    while (!lock_acquired && spin_attempts < MAX_SPIN_ATTEMPTS) {
        if (flock(fd, LOCK_EX | LOCK_NB) == 0) {
            lock_acquired = 1;
        } else {
            // Failed to acquire lock, continue spinning
            spin_attempts++;
        }
    }

    if (!lock_acquired) {
        return -1;
    }

    construct_tree("/home/snapshots/directory_tree.json", PID);
    // Release the lock
    if (flock(fd, LOCK_UN) == -1) {
        return -1;
    }

    close(fd);   


    return 0;
}


/**
 * @brief Thread safe function to dump the directory tree to the json file preserving the file system structure
 * 
 * @return int : -1 on failure, 0 on success 
 */
int extract_tree_to_json(){
    
    int fd = open("/home/snapshots/directory_tree.json", O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        return -1;
    }
    int spin_attempts = 0;
    int lock_acquired = 0;
    
    while (!lock_acquired && spin_attempts < MAX_SPIN_ATTEMPTS) {
        // Attempt to acquire an exclusive lock
        if (flock(fd, LOCK_EX | LOCK_NB) == 0) {
            lock_acquired = 1;
        } else {
            // Failed to acquire lock, continue spinning
            spin_attempts++;
        }
    }

    // printf("here\n");
    if (!lock_acquired) {
        // Max spin attempts reached without acquiring the lock
        return -1;
    }
    // if (directory_tree_head == NULL){
    //     printf("????????????????????????????????????????\n");
    // }
    cJSON* json_tree = dir_tree_to_json(directory_tree_head);

    // printf("here2\n");
    write_json_to_file(json_tree, "/home/snapshots/directory_tree.json");

    // printf("here3\n");


    // Release the lock
    if (flock(fd, LOCK_UN) == -1) {
        return -1;
    } 

    close(fd);   

    return 0;
}


/**
 * @brief The following functions initiallize a spinlock named : json_tree_lock
 * that will be used  to achieve concurrency when writing the tree to the JSON file
 * 
 */

/**
 * @brief Initiallizes the spinlock using the pthread_spin_init function and 
 * updates the init flag for the lock
 */
void init_spinlock_json_tree() {
    pthread_spin_init(&json_tree_lock, 0);
    is_tree_json_lock_initialized = 1;
}
/**
 * @brief Cleans up (destroys) the spinlock instance and set the lock flag to 0
 * 
 */
void destroy_spinlock_json_tree() {
    pthread_spin_destroy(&json_tree_lock);
    is_tree_json_lock_initialized = 0;
}
/**
 * @brief Acquires the spinlock. If not inititallized (based on the flag value)
 * this function initiallizes the lock before trying to acquire it.
 */
void acquire_spinlock_json_tree() {
    if (!is_tree_json_lock_initialized) {
        init_spinlock_json_tree();
    }
    pthread_spin_lock(&json_tree_lock);
}
/**
 * @brief Releases the spinlock
 * 
 */
void release_spinlock_json_tree() {
    pthread_spin_unlock(&json_tree_lock);
}
/**
 * @brief This function is the wrapper function that uses the spinlock: json_tree_lock
 * and preserves consistency when writing the directory tree to the JSON file 
 * 
 */
void tree_write() {    
    acquire_spinlock_json_tree();
    extract_tree_to_json();
    release_spinlock_json_tree();
}







// --------------------------------------------------------------------- WRAPPER FUNCTIONS --------------------------------------------------------------------- //
// Custom version of write
ssize_t my_write(char* file_path, int fd, const void *buf, size_t count, int PID, operation_type type) {
    

    char* full_path = realpath(file_path,NULL);
    int operation_flag = 0;
    switch(type){
        case APPEND:
            // printf("APPEND\n");
            break;
        case WRITE:
            // printf("WRITE\n");
            break;
        default:
            // printf("Unknown\n");
            operation_flag = -1;
            break;
    }
    if( operation_flag == -1){
        fprintf(stderr, "Unknown operation flag [%d] in file %s\nExiting...\n",type,file_path);
        return -1;
    }
    // printf("===================================================================================\n");
    // char command[1024];
    // snprintf(command, sizeof(command), "cat %s", full_path);

    // // Execute the command
    // int result = system(command);
    // if (result == -1) {
    //     perror("Error executing command");
    //     return -1;
    // } else if (result != 0) {
    //     printf("Failed to execute command.\n");
    //     return -1;
    // }


    // printf("===================================================================================\n");
    current_file_path_operated_on = strdup(full_path);

    
    printf("Process :%d writing on %s at [%s]\n",PID,file_path,current_timestamp_as_string());
    if(full_path == NULL){
        printf("No such file or directory %s\n",full_path);
        return -1;
    }else{
        if(directory_tree_head == NULL){
            // printf("NOTHING TO THE TREE YET\n");
            int line_count = 0;
            char** prev_paths = read_absolute_paths(&line_count);
            
            if(prev_paths != NULL){
                // printf("================================================================================================\n");
                // printf("%d\n",line_count);
                for(int i=0; i < line_count; i++){
                    // printf("----------> %s\n",prev_paths[i]);
                    char** path_split = str_split(prev_paths[i],'|');
                    
                    // Get the path to the file from the absolute_paths.txt
                    char* path = realpath(path_split[0],NULL);
                    // Get the corresponding operation performed on that file
                    int operation = atoi(path_split[1]);
                    // printf("FOUND PREVIOUS OPERATION: %d ON FILE: %s\n",operation,path);
                    add_directory_to_tree(path,PID, 0, operation);
                }
                // printf("================================================================================================\n");

            } 
        }
            
        log_write(PID, full_path);
        log_write_abs_path(full_path,(int)(type));
        // printf("PERFORMING %d IN FILE %s\n",(int)(type),full_path);
        add_directory_to_tree(full_path,PID, 1, (int)(type));
    }    
    
    // Call the original write function
    
    ssize_t (*original_write)(int, const void *, size_t) = dlsym(RTLD_NEXT, "write");
    return original_write(fd, buf, count);
}

// Custom version of read
ssize_t my_read(char* file_path, int fd, void *buf, size_t count, int PID, operation_type type) {

    char* full_path = realpath(file_path,NULL);
    if(full_path == NULL){
        printf("No such file or directory %s\n",full_path);
        return -1;
    }else{
        int operation_flag = 0;
        switch(type){
            case READ:
                // printf("APPEND\n");
                break;
            case WRITE:
                // printf("WRITE\n");
                operation_flag = -2;
                break;
            case APPEND:
                // printf("WRITE\n");
                operation_flag = -2;
                break;
            default:
                // printf("Unknown\n");
                operation_flag = -1;
                break;
        }
        if( operation_flag == -1){
            fprintf(stderr, "Unknown operation flag [%d] in file %s\nExiting...\n",type,file_path);
            return -1;
        }
        else if (operation_flag == -2){
            fprintf(stderr, "Unknown operation flag [%d] for reading from file %s\nExiting...\n",type,file_path);
            return -1;
        }else{

            printf("Process :%d reading from %s at [%s]\n",PID,file_path,current_timestamp_as_string());
            if(directory_tree_head == NULL){
                // printf("NOTHING TO THE TREE YET\n");
                int line_count = 0;
                char** prev_paths = read_absolute_paths(&line_count);
            
                if(prev_paths != NULL){
                // printf("================================================================================================\n");
                // printf("%d\n",line_count);
                    for(int i=0; i < line_count; i++){
                        // printf("----------> %s\n",prev_paths[i]);
                        char** path_split = str_split(prev_paths[i],'|');
                        
                        // Get the path to the file from the absolute_paths.txt
                        char* path = realpath(path_split[0],NULL);
                        // Get the corresponding operation performed on that file
                        int operation = atoi(path_split[1]);
                        // printf("FOUND PREVIOUS OPERATION: %d ON FILE: %s\n",operation,path);
                        add_directory_to_tree(path,PID, 0, operation);
                    }
                    // printf("================================================================================================\n");

                } 
            }
        }
        log_write_abs_path(full_path,(int)(type));
        add_directory_to_tree(full_path,PID, 0, (int)(type));
    }

    // Call the original read function
    ssize_t (*original_read)(int, void *, size_t) = dlsym(RTLD_NEXT, "read");
    return original_read(fd, buf, count);
}