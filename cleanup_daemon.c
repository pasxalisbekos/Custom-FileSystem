#include "cleanup_daemon.h"

char* extract_timestamp(char* full_string) {
    
    char* token = strtok(full_string, "_");
    char* sec;
    while (token != NULL) {
        sec = strtok(NULL, "_");
        break;
    }

    char* temp = append_strings(token,"_");
    char* result = append_strings(temp,sec);
    
    return result;
}

char* most_recent_timestamp(char* timestamp1, char* timestamp2) {
    // Compare the date portion
    int date_comparison = strncmp(timestamp1, timestamp2, 10);

    // If the dates are not equal, return the one with the larger date
    if (date_comparison != 0) {
        return (date_comparison > 0) ? timestamp1 : timestamp2;
    }

    // Compare the time portion
    int time_comparison = strncmp(timestamp1 + 11, timestamp2 + 11, 8);

    // If the times are not equal, return the one with the larger time
    if (time_comparison != 0) {
        return (time_comparison > 0) ? timestamp1 : timestamp2;
    }

    // If both dates and times are equal, return either one
    return timestamp1;
}

void remove_old_files(char* directory_path, char* most_recent_snapshot) {
    DIR* dir = opendir(directory_path);
    if (!dir) {
        perror("Unable to open directory");
        exit(EXIT_FAILURE);
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip . and .. directories
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Skip the most recent snapshot
        if (strcmp(entry->d_name, most_recent_snapshot) == 0) {
            continue;
        }

        // Construct the full path of the file
        char file_path[PATH_MAX];
        snprintf(file_path, sizeof(file_path), "%s/%s", directory_path, entry->d_name);

        // Remove the file
        if (remove(file_path) == -1) {
            perror("Unable to remove file");
            exit(EXIT_FAILURE);
        }
    }

    closedir(dir);
}

void list_sub_directories(char *base_directory_path) {
    struct dirent *dp;
    DIR *dir = opendir(base_directory_path);

    // Unable to open directory stream
    if (!dir) {
        printf("Unable to open directory %s\n", base_directory_path);
        return;
    }

    while ((dp = readdir(dir)) != NULL) {
        if (dp->d_type == DT_DIR) {
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
                // printf("=============================================================================================\n");
                // printf("%s/%s\n",base_directory_path, dp->d_name);
                char* base_line_dir = strdup(append_strings(base_directory_path,"/"));
                char* absolute_path = strdup(append_strings(base_line_dir,dp->d_name));
                char* most_recent_snapshot = get_most_recent_snapshot(absolute_path);
                // list_files_in_directory(absolute_path);
                // printf("=============================================================================================\n");
                remove_old_files(absolute_path,most_recent_snapshot);
            }
        }
    }

    closedir(dir);
}

char* get_most_recent_snapshot_for_file(char* file_path){

    char* base_dir = strdup("/home/snapshots/");
    char* hased_dir_name = sha256(realpath(file_path,NULL));

    char* temp_dir = append_strings(base_dir,hased_dir_name);
    char* most_recent_snapshot = get_most_recent_snapshot(temp_dir);
    // printf("Most recent snapshot: %s\n", most_recent_snapshot);
    // char* directory_of_file;
    char* temp_full_dir_path = append_strings(temp_dir,"/");

    char* recent = append_strings(temp_full_dir_path,most_recent_snapshot);
    return recent;
}


void get_all_snapshots_for_file(char* file_name){

    char* real_path = strdup(realpath(file_name,NULL));
    char* baseline_dir = strdup("/home/snapshots/");
    char* hashed_dir_name = strdup(sha256(real_path));
    char* snapshot_dir_name = append_strings(baseline_dir,hashed_dir_name);
    list_files_in_directory(snapshot_dir_name);
}



char* get_snapshot(char* directory, char* snapshot_name) {
    
    // Open the directory
    DIR* dir = opendir(directory);
    if (dir == NULL) {
        perror("Unable to open directory");
        return NULL;
    }

    // Iterate through directory entries
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".." directories
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        
        // Construct the full path of the entry
        char entry_path[PATH_MAX];
        snprintf(entry_path, sizeof(entry_path), "%s/%s", directory, entry->d_name);

        // If the entry is a directory, recursively search within it
        if (entry->d_type == DT_DIR) {
            char* sub_result = get_snapshot(entry_path, snapshot_name);
            if (sub_result != NULL) {
                closedir(dir);
                return sub_result; // Found the file in subdirectory
            }
        } else if (entry->d_type == DT_REG && strcmp(entry->d_name, snapshot_name) == 0) {
            // If the entry is a regular file and matches snapshot_name, return its absolute path
            char* absolute_path = realpath(entry_path, NULL);
            closedir(dir);
            return absolute_path;
        }
    }

    // File not found
    closedir(dir);
    return NULL;
}






void list_files_in_directory(char *directory_path) {
    struct dirent *dp;
    DIR *dir = opendir(directory_path);
    char* recent = "";
    char* most_recent_snapshot;
    // Unable to open directory stream
    if (!dir) {
        printf("Unable to open directory %s\n", directory_path);
        return;
    }

    int size_counter = 0;
    while ((dp = readdir(dir)) != NULL) {
        if (dp->d_type == DT_REG) {
            // printf("%s\n",extract_timestamp(dp->d_name));
            
            char* file_name = strdup(dp->d_name);
            printf("%s\n",file_name);
        }
    }

    closedir(dir);

}

char* get_most_recent_snapshot(char *directory_path) {
    struct dirent *dp;
    DIR *dir = opendir(directory_path);
    char* recent = "";
    char* most_recent_snapshot;
    // Unable to open directory stream
    if (!dir) {
        printf("Unable to open directory %s\n", directory_path);
        return;
    }

    int size_counter = 0;
    while ((dp = readdir(dir)) != NULL) {
        if (dp->d_type == DT_REG) {
            // printf("%s\n",extract_timestamp(dp->d_name));
            
            char* file_name = strdup(dp->d_name);
            char* timestamp = extract_timestamp(dp->d_name);
            if(strlen(recent) == 0){
                recent = strdup(timestamp);
                most_recent_snapshot = strdup(file_name);

            }else {
                recent = strdup(most_recent_timestamp(recent,timestamp));
                if (strcmp(recent,timestamp) == 0){
                    most_recent_snapshot = strdup(file_name);
                }
            }
        }
    }

    closedir(dir);

    return most_recent_snapshot;
}


void init_cleanup_daemon(int flag ,unsigned int time){

    unsigned int seconds;
    switch(flag) {
        case 1:
            printf("Flag is set to seconds [%u]\n", time);
            sleep(time);
            break;
        case 2:
            printf("Flag is set to hours [%u]\n", time);
            seconds = time * 3600;
            sleep(seconds);
            break;
        case 3:
            printf("Flag is set to days [%u]\n", time);
            seconds = time * 24 * 3600;
            sleep(seconds);
            break;
        case 4:
            printf("Flag is set to months [%u] (?)\n", time);
            seconds = time * 30 * 24 * 3600;
            sleep(seconds);
            break;
        default:
            printf("Flag is unrecognised [%d]. Switching to default timelapse of 2 hours for cleanup\n",flag);
    }

    printf("Cleanup operation starting...\n");
    list_sub_directories("/home/snapshots");
    printf("Cleanup operation finished...\n");
}




int revert_to_previous_version(char* current_file_path, char* most_recent_snapshot){

    int current_file_fd = open(current_file_path, O_RDWR);
    if (current_file_fd == -1) {
        perror("Error opening current file");
        return -1;
    }

    int snapshot_file_fd = open(most_recent_snapshot, O_RDWR);
    if (snapshot_file_fd == -1) {
        perror("Error opening snapshot file");
        close(current_file_fd);
        return -1;
    }

    // Lock the files so you ensure noone else is reading or writing at the same time
    if (flock(current_file_fd, LOCK_EX) == -1) {
        perror("Error locking current file");
        close(current_file_fd);
        close(snapshot_file_fd);
        return -1;
    }

    if (flock(snapshot_file_fd, LOCK_EX) == -1) {
        perror("Error locking snapshot file");
        flock(current_file_fd, LOCK_UN); // Release lock on current file
        close(current_file_fd);
        close(snapshot_file_fd);
        return -1;
    }

    // Attempt to remove the existing file
    if (unlink(current_file_path) == -1) {
        perror("Error removing current file");
        flock(current_file_fd, LOCK_UN); // Release lock on current file
        flock(snapshot_file_fd, LOCK_UN); // Release lock on snapshot file
        close(current_file_fd);
        close(snapshot_file_fd);
        return -1;
    }

    char command[4096];
    // Copy most_recent_snapshot to current_file_path
    snprintf(command, sizeof(command), "cp -fp %s %s && sudo chown $(logname) %s", most_recent_snapshot, current_file_path, current_file_path);

    int result = system(command);
    if (result == -1) {
        perror("Error executing cp command");
        flock(current_file_fd, LOCK_UN); // Release lock on current file
        flock(snapshot_file_fd, LOCK_UN); // Release lock on snapshot file
        close(current_file_fd);
        close(snapshot_file_fd);
        return -1;
    } else if (result != 0) {
        printf("Failed to revert file.\n");
        flock(current_file_fd, LOCK_UN); // Release lock on current file
        flock(snapshot_file_fd, LOCK_UN); // Release lock on snapshot file
        close(current_file_fd);
        close(snapshot_file_fd);
        return -1;
    }

    // Release locks and close file descriptors
    flock(current_file_fd, LOCK_UN);
    flock(snapshot_file_fd, LOCK_UN);
    close(current_file_fd);
    close(snapshot_file_fd);


    return 0; // Success
}