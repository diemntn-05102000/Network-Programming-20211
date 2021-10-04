CFLAGS = -c -Wall
CC = gcc
LIBS = -lm
all: HW1
HW1: userManagement.o
	$(CC) userManagement.o -o HW1
userManagement.o: userManagement.c
	$(CC) $(CFLAGS) userManagement.c
clean:
	rm -rf *.o *.~