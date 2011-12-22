# Brian Kocoloski, Cha Li
# Project 1 Makefile

CC = gcc
CC_OPTS = -c

all: dist_system

dist_system: message.o listnode.o distnode.o driver.o
	$(CC) message.o listnode.o distnode.o driver.o -o dist_system

listnode.o: listnode.h listnode.c
	$(CC) $(CC_OPTS) listnode.c

message.o: listnode.h message.h message.c
	$(CC) $(CC_OPTS) message.c

distnode.o: message.h distnode.h listnode.h distnode.c
	$(CC) $(CC_OPTS) distnode.c

driver.o: distnode.h listnode.h message.h driver.c
	$(CC) $(CC_OPTS) driver.c

clean:
	@-rm *.o *.pipe dist_system
