CC=gcc
CFLAGS=-Wall -Wextra -pthread

all: proj2

proj2: main.o process.o shared_data.o
	$(CC) $(CFLAGS) -o proj2 main.o process.o shared_data.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

process.o: process.c process.h
	$(CC) $(CFLAGS) -c process.c

shared_data.o: shared_data.c shared_data.h
	$(CC) $(CFLAGS) -c shared_data.c

clean:
	rm -f *.o proj2