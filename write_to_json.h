#ifndef WRITE_TO_JSON_H
#define WRITE_TO_JSON_H
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <cjson/cJSON.h> 


cJSON* dir_tree_to_json(dir_node* root);
void write_json_to_file(cJSON* json, const char* filename)
#endif
