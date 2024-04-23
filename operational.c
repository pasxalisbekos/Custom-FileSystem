#include "operational.h"


threads_list* global_head = NULL;
pthread_mutex_t head_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief 
 * 
 * @param new_path 
 */
void update_curr_path(char* new_path){
    current_path = strdup(new_path);
}



/**
 * @brief This is the global signal handler that each thread using our custom write/read should first initiallize it. Upon a crash
 * we must be able to preserve the current status of the system (write the tree directory to the JSON file)
 *
 * @param signum : The signal number (e.g. seg fault: 11)
 */
 void custom_signal_handler(int signum) {
    printf("Caught signal %d| Operation path: %s\n", signum, current_path);
    //after here we have to find the nodes and release their locks
    exit(signum);
}

/**
 * @brief This is the initializer function for our signal handler. It sets up our handler for the following signals
 *  1) SIGKILL : Process killed
 *  2) SIGSEGV : Segmentation fault
 *  3) SIGABRT : Abort signal
 *  4) SIGILL  : Illegal instruction
 *  5) SIGFPE  : Floating point exception
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

}

/**
 * @brief This was a custom function that is not currently being user. It simply returned the list of active processes
 * 
 * @return threads_list* 
 */
threads_list* get_linked_list_instance() {
    static threads_list* list = NULL;
    if (list == NULL) {
        pthread_mutex_lock(&head_mutex); // Acquire global mutex
        if (list == NULL) { // Double check list to avoid race condition
            list = (threads_list*)malloc(sizeof(threads_list));
            if (list == NULL) {
                pthread_mutex_unlock(&head_mutex); // Release mutex before returning NULL
                return NULL; // Handle memory allocation failure
            }
            list->head = NULL;
            pthread_mutex_init(&list->mutex, NULL); // Initialize mutex for list
        }
        pthread_mutex_unlock(&head_mutex); // Release global mutex
    }
    return list;
}

/**
 * @brief Adds a new thread instance to the list of threads
 *  
 * @param list : The head of the list 
 * @param PID  : The process id of the process added to the list
 */
void insert_thread(threads_list** list, int PID) {
    if (*list == NULL) {
        *list = get_linked_list_instance();
        if (*list == NULL) {
            return; // Handle initialization failure
        }
    }

    thread_node* newNode = (thread_node*)malloc(sizeof(thread_node));
    if (newNode == NULL) {
        return; // Handle memory allocation failure
    }
    newNode->PID = PID;
    newNode->next = NULL;

    pthread_mutex_lock(&((*list)->mutex));
    if ((*list)->head == NULL) {
        (*list)->head = newNode;
    } else {
        thread_node* temp = (*list)->head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
    pthread_mutex_unlock(&((*list)->mutex));
}

/**
 * @brief Removes a thread instance from the list of threads
 *  
 * @param list : The head of the list 
 * @param PID  : The process id of the process removed from the list
 */
void remove_thread_node(threads_list** list, int PID) {
    if (*list == NULL) {
        return; // Handle null list
    }

    pthread_mutex_lock(&((*list)->mutex));
    thread_node* current = (*list)->head;
    thread_node* prev = NULL;

    if ((*list)->head->next == NULL && (*list)->head->PID == PID) {
        // Handle removal of the head node
        (*list)->head = current->next;
        free(current);
        pthread_mutex_unlock(&((*list)->mutex));
        return;
    }

    // Traverse the list
    while (current != NULL) {
        if (current->PID == PID) {
            if (prev == NULL) {
                (*list)->head = current->next;
                free(current);
                current = (*list)->head;
            } else {
                prev->next = current->next;
                free(current);
                current = prev->next;
            }
        } else {
            prev = current;
            current = current->next;
        }
    }
    pthread_mutex_unlock(&((*list)->mutex));
}


/**
 * @brief Initiallizes the thread list and the signal handler
 * 
 * @param PID : The process ID
 */
void init(int PID){
    //init handler
    handler_init();

    // Insert process node to the list
    insert_thread(&global_head,getpid());

}
/**
 * @brief This function handles the completion of execution. Removes all threads from the lists and prints a log message
 * 
 * @param PID : The process ID
 */
void end(int PID){
    // Remove process node from the list
    remove_thread_node(&global_head,getpid());

    if (global_head->head == NULL){
        printf("Last thread with PID: [%d] finished execution\n",PID);
        // cJSON* json_tree = dir_tree_to_json(directory_tree_head);

        // // Write JSON to file
        // write_json_to_file(json_tree, "directory_tree.json");
    }else{
        printf("Thread [%d] finished execution\n",PID);
    }
}