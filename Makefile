CC = gcc
CFLAGS = -fPIC -Wall -pthread
LDFLAGS = 

.PHONY: build
build: libscheduler.so

libscheduler.so: so_scheduler.o queue.o
	$(CC) $(LDFLAGS) -shared -o $@ $^

so_scheduler.o: so_scheduler.c so_scheduler.h
	$(CC) $(CFLAGS) -o $@ -c $<

queue.o: queue.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	-rm -f queue.o so_scheduler.o libscheduler.so