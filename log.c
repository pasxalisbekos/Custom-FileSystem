#include "log.h"

pthread_spinlock_t lock;
int is_lock_initialized = 0;
/**
 * @brief 
 * 
 */
void init_spinlock() {
    pthread_spin_init(&lock, 0);
    is_lock_initialized = 1;
}
/**
 * @brief 
 * 
 */
void destroy_spinlock() {
    pthread_spin_destroy(&lock);
    is_lock_initialized = 0;
}
/**
 * @brief 
 * 
 */
void acquire_spinlock() {
    if (!is_lock_initialized) {
        init_spinlock();
    }
    pthread_spin_lock(&lock);
}
/**
 * @brief 
 * 
 */
void release_spinlock() {
    pthread_spin_unlock(&lock);
}
/**
 * @brief 
 * 
 * @param pid 
 * @param file_path 
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
 * @brief 
 * 
 */
void init_spinlock_dir_tree() {
    pthread_spin_init(&tree_lock, 0);
    is_tree_lock_initialized = 1;
}
/**
 * @brief 
 * 
 */
void destroy_spinlock_dir_tree() {
    pthread_spin_destroy(&tree_lock);
    is_tree_lock_initialized = 0;
}
/**
 * @brief 
 * 
 */
void acquire_spinlock_dir_tree() {
    if (!is_tree_lock_initialized) {
        init_spinlock_dir_tree();
    }
    pthread_spin_lock(&tree_lock);
}
/**
 * @brief 
 * 
 */
void release_spinlock_dir_tree() {
    pthread_spin_unlock(&tree_lock);
}

/**
 * @brief 
 * 
 * @param file_path 
 */
void log_write_abs_path(char* file_path) {
    char absolute_path[256];

    // printf("here1\n");
    snprintf(absolute_path, sizeof(absolute_path), "%s\n", file_path);
    
    // printf("here2\n");
    acquire_spinlock_dir_tree();
    
    // printf("hereeee\n");
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
 * @brief 
 * 
 * @param line_count 
 * @return char** 
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
        // Remove trailing newline character
        size_t length = strlen(buffer);
        if (buffer[length - 1] == '\n') {
            buffer[length - 1] = '\0';
        }
        lines[i] = strdup(buffer);
        i++;
    }

    *line_count = count; // Update the line count
    fclose(file);

    release_spinlock_dir_tree();
    return lines;
}