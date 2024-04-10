CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: main

main: demo.o custom_filesys.o
	$(CC) $(CFLAGS) -o demo demo.o custom_filesys.o

demo.o: demo.c custom_filesys.h
	$(CC) $(CFLAGS) -c demo.c

custom_filesys.o: custom_filesys.c custom_filesys.h
	$(CC) $(CFLAGS) -c custom_filesys.c

clean:
	rm -f *.o demo