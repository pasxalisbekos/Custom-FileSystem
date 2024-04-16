CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -lssl -lcrypto

all: main

main: demo.o custom_filesys.o cleanup_daemon.o
	$(CC) $(CFLAGS) -o demo demo.o custom_filesys.o cleanup_daemon.o $(LDFLAGS)

demo.o: demo.c custom_filesys.h cleanup_daemon.h
	$(CC) $(CFLAGS) -c demo.c

custom_filesys.o: custom_filesys.c custom_filesys.h
	$(CC) $(CFLAGS) -c custom_filesys.c

cleanup_daemon.o: cleanup_daemon.c cleanup_daemon.h
	$(CC) $(CFLAGS) -c cleanup_daemon.c

clean:
	rm -f *.o demo
