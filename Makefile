CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11 -pthread
LDFLAGS = -lssl -lcrypto -lm -lcjson -pthread -lc

all: main exec snap_ret cleanup

main: demo.o custom_filesys.o cleanup_daemon.o log.o
	$(CC) $(CFLAGS) -o demo demo.o custom_filesys.o cleanup_daemon.o log.o $(LDFLAGS)

exec: exec_operation.o custom_filesys.o cleanup_daemon.o log.o
	$(CC) $(CFLAGS) -o exec exec_operation.o custom_filesys.o cleanup_daemon.o log.o $(LDFLAGS)

snap_ret: retrieve_snapshot.o custom_filesys.o cleanup_daemon.o log.o
	$(CC) $(CFLAGS) -o snap_ret retrieve_snapshot.o custom_filesys.o cleanup_daemon.o log.o $(LDFLAGS)

cleanup: cleanup.o custom_filesys.o cleanup_daemon.o log.o
	$(CC) $(CFLAGS) -o cleanup cleanup.o custom_filesys.o cleanup_daemon.o log.o $(LDFLAGS)

exec_operation.o: exec_operation.c custom_filesys.h cleanup_daemon.h log.h
	$(CC) $(CFLAGS) -c exec_operation.c

demo.o: demo.c custom_filesys.h cleanup_daemon.h log.h
	$(CC) $(CFLAGS) -c demo.c

retrieve_snapshot.o: retrieve_snapshot.c custom_filesys.h cleanup_daemon.h log.h
	$(CC) $(CFLAGS) -c retrieve_snapshot.c

cleanup.o: cleanup.c custom_filesys.h cleanup_daemon.h log.h
	$(CC) $(CFLAGS) -c cleanup.c

custom_filesys.o: custom_filesys.c custom_filesys.h
	$(CC) $(CFLAGS) -c custom_filesys.c

cleanup_daemon.o: cleanup_daemon.c cleanup_daemon.h
	$(CC) $(CFLAGS) -c cleanup_daemon.c

log.o: log.c log.h
	$(CC) $(CFLAGS) -c log.c


clean:
	rm -f *.o demo exec snap_ret cleanup
