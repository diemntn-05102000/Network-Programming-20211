CFLAGS = -c -Wall
CC = gcc
LIBS = -lm
all: HW2
HW2: resolver.o
	$(CC) resolver.o -o resolver
resolver.o: resolver.c
	$(CC) $(CFLAGS) resolver.c
clean:
	rm -rf *.o *.~