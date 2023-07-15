#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "shared_data.h"
#include "process.h"

//function for process customer
void process_customer(int id, int TZ, shared_data_t *shared_data)
{
    srand(time(NULL) ^ getpid() << 16); //key for random, shifting 16 bits for better random
    int is_open = 0;

    sem_wait(shared_data->mutex);
    fprintf(shared_data->file, "%d: Z %d: started\n", ++shared_data->action_counter, id);
    fflush(shared_data->file);
    sem_post(shared_data->mutex);

    if(TZ > 0)
    {
        usleep((rand() % TZ) * 1000); //wait before customer come in to office
    }

    sem_wait(shared_data->mutex);
    is_open = shared_data->office_open; //using for savety calling from shared memmory (anti deadlock)
    sem_post(shared_data->mutex);

    if(is_open)
    {
        int X = rand() % MAX_QUEUE;
        sem_wait(shared_data->mutex);
        fprintf(shared_data->file, "%d: Z %d: entering office for a service %d\n", ++shared_data->action_counter, id, X + 1);
        fflush(shared_data->file);
        shared_data->queue_sizes[X]++; //customer come so increamenting the queue
        sem_post(shared_data->mutex);

        sem_wait(shared_data->call_customer_sem[X]); //customer wait before clerk call him

        //clerk call customer
        sem_wait(shared_data->mutex);
        fprintf(shared_data->file, "%d: Z %d: called by office worker\n",++shared_data->action_counter, id);
        fflush(shared_data->file);
        shared_data->queue_sizes[X]--; //customer come to clerk so decrementing the queue where was customer
        sem_post(shared_data->mutex);

        usleep((rand() % 10) * 1000); //waitng time before customer leave
    }
    sem_wait(shared_data->mutex);
    fprintf(shared_data->file, "%d: Z %d: going home\n", ++shared_data->action_counter, id);
    sem_post(shared_data->mutex);
    exit(0); 
}

//function of clerk process
void process_clerk(int id, int TU, shared_data_t *shared_data)
{   
    srand(time(NULL) ^ getpid() << 16); //key for random, shifting 16 bits for better random

    sem_wait(shared_data->mutex);
    fprintf(shared_data->file, "%d: U %d: started\n", ++shared_data->action_counter, id);
    sem_post(shared_data->mutex);

    while(1)
    {
        sem_wait(shared_data->mutex);
        //clerk searching for customer in queue 
        int X = -1;
        for(int i = 0; i < MAX_QUEUE; i++)
        {
            if(shared_data->queue_sizes[i] > 0)
            {
                X = i;
                break;
            }
        }
        int is_open = shared_data->office_open;
        sem_post(shared_data->mutex);
        //if clerk find customer
        if(X != -1)
        {
            sem_post(shared_data->call_customer_sem[X]); //clerk call customer

            sem_wait(shared_data->mutex);
            fprintf(shared_data->file, "%d: U %d: serving a service of type %d\n", ++shared_data->action_counter, id, X + 1);
            fflush(shared_data->file);
            sem_post(shared_data->mutex);

            usleep((rand() % 10) * 1000); //time for serving a service 

            sem_wait(shared_data->mutex);
            fprintf(shared_data->file, "%d: U %d: service finished\n", ++shared_data->action_counter, id);
            fflush(shared_data->file);
            sem_post(shared_data->mutex);
        }
        //if customer not found in queue but office is open
        else if(is_open)
        {
            //clerk takes a break
            sem_wait(shared_data->mutex);
            fprintf(shared_data->file, "%d: U %d: taking break\n", ++shared_data->action_counter, id);
            fflush(shared_data->file);
            sem_post(shared_data->mutex);
            if(TU > 0)
            {
                usleep((rand() % TU ) * 1000); //how long does it take for clerk to rest 
            }
            sem_wait(shared_data->mutex);
            fprintf(shared_data->file, "%d: U %d: break finished\n", ++shared_data->action_counter, id);
            fflush(shared_data->file);
            sem_post(shared_data->mutex);
        }
        //if clerk doesnt find anyone in 3 queues and the office close
        else
        {
            //going home
            sem_wait(shared_data->mutex);
            fprintf(shared_data->file, "%d: U %d: going home\n", ++shared_data->action_counter, id);
            fflush(shared_data->file);
            sem_post(shared_data->mutex);

            exit(0);
        }
    }
}

//creating a list of pids
process_list_t* create_process_list(int capacity) 
{
    process_list_t *list = malloc(sizeof(process_list_t));
    list->pids = malloc(capacity * sizeof(pid_t));
    list->count = 0;
    list->capacity = capacity;
    return list;
}

//destroys a list of pids 
void destroy_process_list(process_list_t *list) 
{
    if (list) 
    {
        free(list->pids);
        free(list);
    }
}

//adding a pid to list of pids
void add_process(process_list_t *list, pid_t pid) 
{
    if (list->count < list->capacity) 
    {
        list->pids[list->count++] = pid;
    }
}

//killing all processes and cleaning up memmory (using only for errors!)
void terminate_processes_and_cleanup(process_list_t *list, shared_data_t *shared_data, FILE *file) 
{
    for (int i = 0; i < list->count; i++) 
    {
        kill(list->pids[i], SIGTERM);
    }
    while (wait(NULL) > 0);

    cleanup(shared_data, file);
    destroy_process_list(list);
}