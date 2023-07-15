#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <sys/wait.h>

#include "shared_data.h"
#include "process.h"

const char *FILENAME = "proj.out";

int main(int argc, char *argv[]) 
{
    srand(time(NULL));
    //work with arguments
    if (argc != 6) {
        fprintf(stderr, "Error: Invalid number of arguments.\n");
        return 1;
    }
    char *endptr;
    errno = 0;

    int NZ = (int)strtol(argv[1], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        fprintf(stderr, "Error: Invalid argument value for NZ.\n");
        return 1;
    }

    int NU = (int)strtol(argv[2], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        fprintf(stderr, "Error: Invalid argument value for NU.\n");
        return 1;
    }

    int TZ = (int)strtol(argv[3], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        fprintf(stderr, "Error: Invalid argument value for TZ.\n");
        return 1;
    }

    int TU = (int)strtol(argv[4], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        fprintf(stderr, "Error: Invalid argument value for TU.\n");
        return 1;
    }

    int F = (int)strtol(argv[5], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        fprintf(stderr, "Error: Invalid argument value for F.\n");
        return 1;
    }

    if (NZ < 0 || NU <= 0 || TZ < 0 || TZ > 10000 || TU < 0 || TU > 100 || F < 0 || F > 10000) {
        fprintf(stderr, "Error: Invalid argument values.\n");
        return 1;
    }

    FILE *file = fopen(FILENAME, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: Failed to open file.\n");
        return 1;
    }
    
    //creating shared memory
    shared_data_t *shared_data = create_shared_data();
    initialize_shared_data(shared_data, file, F); // initializing shared mamory 

    int max_processes = NZ + NU;
    process_list_t *process_list = create_process_list(max_processes); //creating list of pids

    //starting processes for customer 
    for(int i = 1; i <= NZ; i++)
    {
        pid_t pid = fork();
        if(pid < 0)
        {
            fprintf(stderr, "Error: fork failed\n");
            terminate_processes_and_cleanup(process_list, shared_data, file); //terminating all processes if fork return error 
            exit(1);
        }
        else if(pid == 0)
        {
            process_customer(i, TZ, shared_data);
            exit(0);
        }
        else
        {
            add_process(process_list, pid);
        }
    } 

    //starting all processes for clerk 
    for(int i = 1; i <= NU; i++)
    {
        pid_t pid = fork();
        if(pid < 0)
        {
            fprintf(stderr, "Error: fork failed\n");
            terminate_processes_and_cleanup(process_list, shared_data, file); //terminating all processes if fork returns error    
            exit(1);
        }
        else if(pid == 0)
        {
            process_clerk(i, TZ, shared_data);
            exit(0);
        }
        else 
        {
            add_process(process_list, pid);
        }
    } 

    //wait random time before the office close
    if (F > 0) 
    {
        usleep(((rand() % (F / 2)) + F / 2)*1000);
    }

    sem_wait(shared_data->mutex);
    shared_data->office_open = 0;
    fprintf(shared_data->file, "%d: closing\n", ++shared_data->action_counter);
    fflush(shared_data->file);
    sem_post(shared_data->mutex);

    while(wait(NULL) > 0); //waiting for ending of all processes 

    fclose(file);
    destroy_shared_data(shared_data); //destroy all shared memory
    destroy_process_list(process_list); //destroy list of pids 

    return 0;
}
