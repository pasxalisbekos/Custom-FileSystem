#include "log.h"

pthread_spinlock_t lock;
int is_lock_initialized = 0;
/**
 * @brief Initiallizes the log file's spinlock and sets up the lock_is_initialized flag to 1
 * 
 */
void init_spinlock() {
    pthread_spin_init(&lock, 0);
    is_lock_initialized = 1;
}
/**
 * @brief Cleans up (destroys) the log file's spinlock and sets up the lock_is_initialized flag to 0
 * 
 */
void destroy_spinlock() {
    pthread_spin_destroy(&lock);
    is_lock_initialized = 0;
}
/**
 * @brief Acquires the log file's spinlock, if not initiallized then first initiallizes it
 * and updates the lock_is_initialized flag to 1
 */
void acquire_spinlock() {
    if (!is_lock_initialized) {
        init_spinlock();
    }
    pthread_spin_lock(&lock);
}
/**
 * @brief Releases the log file's spinlock
 * 
 */
void release_spinlock() {
    pthread_spin_unlock(&lock);
}
/**
 * @brief This function preserves a log file under the directory /home/snapshots named log.txt that records the process ID
 * ,the operation the process performed and the file where this operation was performed.
 * @param pid : The process performing an operation (read/write)
 * @param file_path : The path to the file where the operation takes place
 */
void log_write(int pid, char* file_path) {
    char log_message[256];
    snprintf(log_message, sizeof(log_message), "Process with PID: %d performed a write operation on file: %s\n", pid, file_path);
    
    acquire_spinlock();
    int log_fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd == -1) {
        perror("Error opening log file");
        release_spinlock();
        return;
    }

    write(log_fd, log_message, strlen(log_message));

    close(log_fd);
    release_spinlock();
}


/*-------------------------------------------------------------- TREE DIRECTORY LOCKING FOR READING AND WRITING ---------------------------------------------------------*/

pthread_spinlock_t tree_lock;
int is_tree_lock_initialized = 0;

/**
 * @brief Initiallizes the absolute paths log file's spinlock and sets up the is_tree_lock_initialized flag to 1
 * 
 */
void init_spinlock_dir_tree() {
    pthread_spin_init(&tree_lock, 0);
    is_tree_lock_initialized = 1;
}
/**
 * @brief Cleans up (destroys) the absolute paths log file's spinlock and sets up the is_tree_lock_initialized flag to 0
 * 
 */
void destroy_spinlock_dir_tree() {
    pthread_spin_destroy(&tree_lock);
    is_tree_lock_initialized = 0;
}
/**
 * @brief Acquires the absolute paths log file's spinlock, if not initiallized then first initiallizes it
 * and updates the is_tree_lock_initialized flag to 1
 */
void acquire_spinlock_dir_tree() {
    if (!is_tree_lock_initialized) {
        init_spinlock_dir_tree();
    }
    pthread_spin_lock(&tree_lock);
}
/**
 * @brief Releases the absolute paths log file's spinlock 
 * 
 */
void release_spinlock_dir_tree() {
    pthread_spin_unlock(&tree_lock);
}

/**
 * @brief This function is used to write the absolute path of a file where an operation is taking place to a log file preserving
 * all paths to files that where modified. This might require a cleanup (e.g. reserve unique paths only but this will be handled)
 * by the daemon process
 * @param file_path 
 */
void log_write_abs_path(char* file_path,int type) {
    char absolute_path[256];
    // printf("===============================> %d\n",type);
    snprintf(absolute_path, sizeof(absolute_path), "%s|%d\n", file_path, type);
    
    acquire_spinlock_dir_tree();
    
    int log_fd = open(ABSOLUTE_PATHS, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd == -1) {
        perror("Error opening log file");
        release_spinlock_dir_tree();
        return;
    }
    write(log_fd, absolute_path, strlen(absolute_path));
    
    close(log_fd);
    release_spinlock_dir_tree();

}


/**
 * @brief This is a thread safe function that is used to read all paths reserved in the absolute paths log file.
 * 
 * @param line_count : The number of lines read
 * @return char** : An array of string including all preserved paths.
 */
char** read_absolute_paths(int* line_count) {

    acquire_spinlock_dir_tree();
    FILE* file = fopen(ABSOLUTE_PATHS, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", ABSOLUTE_PATHS);
        release_spinlock_dir_tree();
        return NULL;
    }

    // Count the number of lines in the file
    int count = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            count++;
        }
    }
    rewind(file); // Reset file pointer to the beginning of the file

    // Allocate memory for the array of strings
    char** lines = (char**)malloc(count * sizeof(char*));
    if (lines == NULL) {
        fclose(file);
        fprintf(stderr, "Memory allocation error\n");
        release_spinlock_dir_tree();
        return NULL;
    }

    // Read each line from the file
    char buffer[MAX_LINE_LENGTH];
    int i = 0;
    while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL) {
        size_t length = strlen(buffer);
        if (buffer[length - 1] == '\n') {
            buffer[length - 1] = '\0';
        }
        lines[i] = strdup(buffer);
        i++;
    }

    *line_count = count;
    fclose(file);

    release_spinlock_dir_tree();
    return lines;
}
