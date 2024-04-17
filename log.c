#include "log.h"

pthread_spinlock_t lock;

void init_spinlock() {
    pthread_spin_init(&lock, 0);
}

void destroy_spinlock() {
    pthread_spin_destroy(&lock);
}

void acquire_spinlock() {
    pthread_spin_lock(&lock);
}

void release_spinlock() {
    pthread_spin_unlock(&lock);
}

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
