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
                printf("=============================================================================================\n");
                printf("%s/%s\n",base_directory_path, dp->d_name);
                char* base_line_dir = strdup(append_strings(base_directory_path,"/"));
                char* absolute_path = strdup(append_strings(base_line_dir,dp->d_name));
                char* most_recent_snapshot = list_files_in_directory(absolute_path);
                printf("Most recent snapshot: %s\n", most_recent_snapshot);
                printf("=============================================================================================\n");
                remove_old_files(absolute_path,most_recent_snapshot);
            }
        }
    }

    closedir(dir);
}

char* list_files_in_directory(char *directory_path) {
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
        case 0:
            printf("Flag is set to seconds [%u]\n", time);
            sleep(time);
            break;
        case 1:
            printf("Flag is set to hours [%u]\n", time);
            seconds = time * 3600;
            sleep(seconds);
            break;
        case 2:
            printf("Flag is set to days [%u]\n", time);
            seconds = time * 24 * 3600;
            sleep(seconds);
            break;
        case 3:
            printf("Flag is set to months [%u] (?)\n", time);
            seconds = time * 30 * 24 * 3600;
            sleep(seconds);
            break;
        default:
            printf("Flag is unrecognised [%d]. Switching to default timelapse of 2 hours for cleanup\n",flag);
    }

    list_sub_directories("/home/snapshots");

}