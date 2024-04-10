#include "custom_filesys.h"

char** str_split(char* file_path, const char delimeter)
{
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


// Custom version of write
ssize_t my_write(char* file_path, int fd, const void *buf, size_t count, int PID) {
    
    printf("=============================================================================\n");
    printf("File path: %s\n", file_path);
    printf("File descriptor value: %d\n",fd);
    printf("Contents to write: %s\n", (char*)(buf));
    printf("PID OF PROCESS PERFORMING WRITE: [%d]\n",PID);
    printf("=============================================================================\n");
    
    
    char** tokens;
    int token_index;

    tokens = str_split(file_path, '/');
    
    if(tokens){
        for(token_index = 0; tokens[token_index+1]; token_index++){
            printf("%s\n",tokens[token_index]);
        }
    }else{  
        // If there is no path (e.g. "file.txt" instead of /example/directory/path/file.txt)
        // then we need to get the current working directory and append the filename at the end in 
        // order to retrieve the full path of the file
        printf("Error getting tokens\n");
        // printf("%s\n",getcwd(".", _PC_PATH_MAX));
        long size;
        char *buf;
        char *ptr;

        size = pathconf(".", _PC_PATH_MAX);
        if ((buf = (char *)malloc((size_t)size)) != NULL){
            ptr = getcwd(buf, (size_t)size);
        
            char* test = append_strings(ptr,append_strings("/",file_path));
            tokens = str_split(test, '/');
    
            if(tokens){
                for(token_index = 0; tokens[token_index+1]; token_index++){
                    printf("%s\n",tokens[token_index]);
                }
            }else{
                printf("Could not get directory for this file\n");
            } 
        }
    }
    
    
    
    // Call the original write function
    ssize_t (*original_write)(int, const void *, size_t) = dlsym(RTLD_NEXT, "write");
    return original_write(fd, buf, count);
}

// Custom version of read
ssize_t my_read(char* file_path, int fd, void *buf, size_t count, int PID) {
    
    printf("=============================================================================\n");
    printf("File path: %s\n", file_path);
    printf("File descriptor value: %d\n",fd);
    // printf("Contents read: %s\n", (char*)(buf));
    printf("PID OF PROCESS PERFORMING READ: [%d]\n",PID);
    printf("=============================================================================\n");









    // Call the original read function
    ssize_t (*original_read)(int, void *, size_t) = dlsym(RTLD_NEXT, "read");
    return original_read(fd, buf, count);
}