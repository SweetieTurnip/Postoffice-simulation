#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdarg.h>

#include "shared_data.h"

//destroy shared memmory and closes file
void cleanup(shared_data_t *shared_data, FILE *file) 
{
    if (shared_data != NULL) 
    {
        destroy_shared_data(shared_data);
    }
    if (file != NULL) 
    {
        fclose(file);
    }
}

//destroys shared memmory
void destroy_shared_data(shared_data_t *data) {
    sem_close(data->mutex);
    sem_unlink("/xposte00.mutex");
    for (int i = 0; i < MAX_QUEUE; i++) {
        char call_customer_sem_name[32];
        snprintf(call_customer_sem_name, sizeof(call_customer_sem_name), "/xposte00.call_customer_sem%d", i);

        sem_close(data->call_customer_sem[i]);
        sem_unlink(call_customer_sem_name);
    }
    munmap(data, sizeof(shared_data_t));
}

//creating shared memory
shared_data_t *create_shared_data(void) {
    shared_data_t *data = mmap(NULL, sizeof(shared_data_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    if(data == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }
   return data;
}

//initializing shared memmory 
void initialize_shared_data(shared_data_t *data, FILE *file, int F) {
    data->action_counter = 0;
    sem_unlink("/xposte00.mutex");
    data->mutex = sem_open("/xposte00.mutex", O_CREAT, 0644, 1);
    if (data->mutex == SEM_FAILED) 
    {
        perror("sem_open failed");
        munmap(data, sizeof(shared_data_t));
        exit(1);
    }
    for (int i = 0; i < MAX_QUEUE; i++) 
    {
        char call_customer_sem_name[32];
        snprintf(call_customer_sem_name, sizeof(call_customer_sem_name), "/xposte00.call_customer_sem%d", i);

        sem_unlink(call_customer_sem_name);
        data->call_customer_sem[i] = sem_open(call_customer_sem_name, O_CREAT, 0644, 0);
        if (data->call_customer_sem[i] == SEM_FAILED) 
        {
            perror("sem_open failed");
            cleanup(data, file);
            exit(1);
        }
    }
    for(int i = 0; i < MAX_QUEUE; i++)
    {
        data->queue_sizes[i] = 0;
    }
    data->office_open = 0;
    if (F > 0)
    {
        data->office_open = 1;
    }
    data->file = file;
}