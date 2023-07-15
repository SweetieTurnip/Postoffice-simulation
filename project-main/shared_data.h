#ifndef SHARED_DATA
#define SHARED_DATA

#include <stdlib.h>
#include <semaphore.h>
#include <stdbool.h>

#define MAX_QUEUE 3 //maximum number of queues

//structure for shared memmory
typedef struct
{
    int action_counter;
    sem_t *mutex;
    sem_t *call_customer_sem[MAX_QUEUE];
    int queue_sizes[MAX_QUEUE];
    int office_open;
    FILE *file;
} shared_data_t;


shared_data_t *create_shared_data(void);

void initialize_shared_data(shared_data_t *data, FILE *file, int F); 

void destroy_shared_data(shared_data_t *data);

void cleanup(shared_data_t *shared_data, FILE *file); 

#endif
