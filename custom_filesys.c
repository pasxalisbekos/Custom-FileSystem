#include "custom_filesys.h"


dir_node* directory_tree_head = NULL;

// --------------------------------------------------------------------- HELPER FUNCTIONS --------------------------------------------------------------------- //

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

    // Get the current time
    time(&current_time);
    time_info = localtime(&current_time);

    // Format the time as a string
    strftime(timestamp, MAX_TIMESTAMP_LENGTH, "%Y-%m-%d_%H:%M:%S", time_info);

    return timestamp;
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

    //2) Snapshot directory creation/search
    struct stat st = {0};
    char* baseline_dir = strdup("/home/snapshots/");
    char* hashed_dir_name = strdup(sha256(absolute_path));
    char* snapshot_dir_name = append_strings(baseline_dir,hashed_dir_name);

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
void update_only_snapshot(snapshot** head, char* file_name, char* absolute_path){

    snapshot* new_snapshot = (snapshot*)malloc(sizeof(snapshot));
    if (new_snapshot == NULL){
        return;
    }
    
    
    char* curr_timestamp = current_timestamp_as_string();
    char* snapshot_dir = create_snapshot(file_name,absolute_path,curr_timestamp);
    new_snapshot->snapshot_path = strdup(snapshot_dir);
    new_snapshot->timestamp = strdup(curr_timestamp);

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
void push(file_node** head_ref, char* file_name, char* absolute_path) {
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
    new_file->versions_path = NULL;

    // Initially just store the path to the snapshots and the timestamp
    char* snapshot_dir = create_snapshot(file_name,absolute_path,curr_timestamp);
    new_file->snapshot = (snapshot*)malloc(sizeof(snapshot));
    new_file->snapshot->snapshot_path = strdup(snapshot_dir);
    new_file->snapshot->timestamp = strdup(curr_timestamp);
    new_file->snapshot->next = NULL;

    new_file->next = *head_ref;
    
    *head_ref = new_file;
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
void add_directory_to_tree(char* absolute_path) {
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
        push(&(current_node->files_list), file , absolute_path);
    } else {
        // We must only update the snapshots for this file
        update_only_snapshot(&(temp->snapshot), file, absolute_path);
        printf("%s\n",file);
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
    if(head != NULL){
        print_list(head);
    }
    // Recursively print children
    if (node->children != NULL) {
        int i = 0;
        while (node->children[i] != NULL) {
            print_tree_recursive(node->children[i], depth + 1);
            i++;
        }
    }
}


// --------------------------------------------------------------------- WRAPPER FUNCTIONS --------------------------------------------------------------------- //
// Custom version of write
ssize_t my_write(char* file_path, int fd, const void *buf, size_t count, int PID) {
    
    char* full_path = realpath(file_path,NULL);
    if(full_path == NULL){
        printf("No such file or directory %s\n",full_path);
        return -1;
    }else{
        add_directory_to_tree(full_path);
    }    
    
    // Call the original write function
    ssize_t (*original_write)(int, const void *, size_t) = dlsym(RTLD_NEXT, "write");
    return original_write(fd, buf, count);
}

// Custom version of read
ssize_t my_read(char* file_path, int fd, void *buf, size_t count, int PID) {
    

    char* full_path = realpath(file_path,NULL);
    if(full_path == NULL){
        printf("No such file or directory %s\n",full_path);
        return -1;
    }else{
        printf("=============================================================================\n");
        printf("File path: %s\n", full_path);
        printf("File descriptor value: %d\n",fd);
        // printf("Contents read: %s\n", (char*)(buf));
        printf("PID OF PROCESS PERFORMING READ: [%d]\n",PID);
        printf("=============================================================================\n");
    }








    // Call the original read function
    ssize_t (*original_read)(int, void *, size_t) = dlsym(RTLD_NEXT, "read");
    return original_read(fd, buf, count);
}