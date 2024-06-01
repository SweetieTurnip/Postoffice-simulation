# Post office simulation

This program simulates a customer-clerk system where customers interact with clerks through shared memory and semaphores. It allows customers to enter an office, request a service, and be served by available clerks. Was made for a second IOS project to practice multiprocess programming.

## Features

- Multiple customers and clerks can be created as separate processes.
- Customers wait in queues for available clerks.
- Clerks serve customers one at a time.
- Customers and clerks communicate through shared memory and semaphores.
- Randomized waiting times for customers and clerks can be specified.

## Usage

1. Compile the program using the provided makefile:
   ```shell
   $ make
2. Run the program with the following command-line arguments:
   ```shell
   $ ./customer_clerk NZ NU TZ TU F

  NZ: Number of customer processes to create.
  NU: Number of clerk processes to create.
  TZ: Maximum waiting time for customers (in milliseconds).
  TU: Maximum resting time for clerks (in milliseconds).
  F: Maximum time before the office closes (in milliseconds).
  
Example usage: ./customer_clerk 5 3 1000 500 5000

## Dependencies
The program requires a C compiler, such as GCC, to build the executable.
The program uses standard C libraries and POSIX libraries for inter-process communication.

## Examples
Example 1: Simulating a customer-clerk system with 5 customers, 3 clerks, maximum customer wait time of 1000ms, maximum clerk rest time of 500ms, and an office closing time of 5000ms.

./customer_clerk 5 3 1000 500 5000

## Additional Notes
Ensure that you have the necessary permissions to compile and run the program.
The program creates an output file named "proj.out" to store the simulation results.

