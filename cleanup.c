#include "custom_filesys.h"
#include "cleanup_daemon.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, char* argv[]){

    if(argc != 3){
        printf("Invalid arguments. Execution: sudo ./cleanup <time> <flag>\n Where time is the timeframe to wait before cleaning up snapshot directories\n And flag is:\n {*} 1 : seconds\n {*} 2 : hours\n {*} 3 : days\n {*} 4 : years\n");
        return 0;
    }
    else{
        int flag = atoi(argv[2]);
        int sleep_time = atoi(argv[1]);
        if (flag == 0 || sleep_time == 0){
            printf("Invalid arguments\n");
            return 0;
        }else{
            init_cleanup_daemon(flag,(unsigned int)sleep_time);
        }
    }
    return 0;
}