#include "custom_filesys.h"


dir_node* directory_tree_head = NULL;

// --------------------------------------------------------------------- HELPER FUNCTIONS --------------------------------------------------------------------- //
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

char* get_last_element(char** array) {
    char* last_element = NULL;
    for (int i = 0; array[i] != NULL; i++) {
        last_element = array[i];
    }
    return last_element;
}

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

char* replace_char(char* str, char find, char replace){
    char *current_pos = strchr(str,find);
    while (current_pos) {
        *current_pos = replace;
        current_pos = strchr(current_pos,find);
    }
    return str;
}
// --------------------------------------------------------------------- FILE LIST FUNCTIONS --------------------------------------------------------------------- //
void create_snapshot(char* file_name, char* absolute_path){
    /**
        Here I defined how we store the snapshot of a file to the appropriate directory
        1) Let's assume that all snapshots are saved in the /home/snapshot directory (we need to initiallize its creation somewhere most likely in the first read or write operation)
        2) Now, every file should be stored to a sub-directory which has as name the hash of the path 
            a) Assume path/to/temp/file.txt is the path
            b) The name of directory where all snapshots of this file are stored is: /home/snapshots/hash(path/to/temp/file.txt)
            c) If there is no such directory -> create it
                Else store the snapshot in that directory
        3) Snapshot name is: timestamp_filename
    */

    //1)  Snapshot file name creation e.g. 2024-04-16 13:23:25_pass.txt
    char* temp_filename = strdup(append_strings("_",file_name));
    char* snapshot_file_name = strdup(append_strings(current_timestamp_as_string(),temp_filename));

    //2) Snapshot directory creation/search
    struct stat st = {0};
    char* baseline_dir = strdup("/home/snapshots/");

    char* hashed_dir_name = strdup(sha256(absolute_path));

    char* snapshot_dir_name = append_strings(baseline_dir,hashed_dir_name);
    // printf("%s\n",snapshot_dir_name);
    if (stat(snapshot_dir_name, &st) == -1) {
        //printf("Directory with snapshots for file %s does not exist\n",file_name);
        int temp = mkdir(snapshot_dir_name, 0777);
        //printf("%d\n",temp);
        if (temp != 0){
            printf("Error creating snapshot directory\n");
        }
    }   
    

    // 3) Create the absolute path to the snapshot e.g. --> /home/snapshots/9b24f0a105528cc2fab1c1324299ebcf965dd35cc83114623604dd214175eb29/2024-04-16 13:42:26_pass.txt
    char* temp_snap_dir = append_strings(snapshot_dir_name,"/");
    char* snapshot_absolute_path = append_strings(temp_snap_dir,snapshot_file_name);

    // printf("%s\n",snapshot_absolute_path);
    // printf("%s\n",absolute_path);
    // 4) Now create a snapshot of the original file
    char command[1024];
    //                                            source_path       destination_path
    snprintf(command, sizeof(command), "cp %s %s", absolute_path, snapshot_absolute_path);

    int status = system(command);
    if (status == -1) {
        perror("Error executing system command");
        return 1;
    } else if (status != 0) {
        fprintf(stderr, "Command failed with exit status %d\n", status);
        return 1;
    }

}


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


    new_file->file_hash = strdup(sha256(file_name));
    new_file->versions_path = NULL;


    create_snapshot(file_name,absolute_path);

    new_file->next = *head_ref;
    
    *head_ref = new_file;
}

void print_list(file_node* head){
    
    file_node* curr = head;

    printf("\t\t\t[");
    while (curr != NULL)
    {
        printf("%s",curr->file_name);
        curr = curr->next;
        if(curr != NULL){
            printf(", ");
        }
    }
    printf("]\n");
}

// --------------------------------------------------------------------- DIRECTORY FUNCTIONS --------------------------------------------------------------------- //

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
            // printf("===================> %s\n",file);
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


    // printf("Last dir: %s -----> File: %s\n",last_directory->dir_name,file);
    push(&(current_node->files_list), file , absolute_path);
    free(path_copy); // Free the duplicated path
}

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
    // printf("Operation on file: %s\nat %s\n",file_path,current_timestamp_as_string());
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