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
 * @brief 
 * 
 * @param signum 
 */
void signal_handler(int signum) {
    printf("Caught signal %d\n", signum);
    exit(signum);
}
/**
 * @brief 
 * 
 * @param signum 
 */
void custom_signal_handler(int signum) {
    printf("Caught signal %d| Operation path: %s\n", signum, current_path);
    //after here we have to find the nodes and release their locks
    exit(signum);
}

/**
 * @brief 
 * 
 */
void handler_init(){
    // Setting up our signal handler for all those signals
    signal(SIGKILL, custom_signal_handler); // process killed
    signal(SIGSEGV, custom_signal_handler); // Segmentation fault
    signal(SIGABRT, custom_signal_handler); // Abort signal
    signal(SIGILL,  custom_signal_handler);  // Illegal instruction
    signal(SIGFPE,  custom_signal_handler);  // Floating point exception

}

/**
 * @brief Get the Linked List Instance object
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
 * @brief 
 * 
 * @param list 
 * @param PID 
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
 * @brief 
 * 
 * @param list 
 * @param PID 
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
 * @brief 
 * 
 * @param PID 
 */
void init(int PID){
    //init handler
    handler_init();

    // Insert process node to the list
    insert_thread(&global_head,getpid());

}
/**
 * @brief 
 * 
 * @param PID 
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