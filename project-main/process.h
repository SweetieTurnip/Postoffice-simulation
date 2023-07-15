#ifndef PROCESS
#define PROCESS

#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#include "shared_data.h"

//structure for list of pids
typedef struct 
{
    pid_t *pids;
    int count;
    int capacity;
} process_list_t;

void process_customer(int id, int TZ, shared_data_t *shared_data);

void process_clerk(int id, int TU, shared_data_t *shared_data);

process_list_t* create_process_list(int capacity);

void destroy_process_list(process_list_t *list);

void add_process(process_list_t *list, pid_t pid);

void terminate_processes_and_cleanup(process_list_t *list, shared_data_t *shared_data, FILE *file);

#endif
