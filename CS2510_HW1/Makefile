# Brian Kocoloski, Cha Li
# Project 1 Makefile

CC = gcc
CC_OPTS = -c

all: master_slave

master_slave: message.o listnode.o distnode.o master_slave.o
	$(CC) message.o listnode.o distnode.o master_slave.o -o master_slave

listnode.o: listnode.h listnode.c
	$(CC) $(CC_OPTS) listnode.c

message.o: listnode.h message.h message.c
	$(CC) $(CC_OPTS) message.c

distnode.o: message.h distnode.h listnode.h distnode.c
	$(CC) $(CC_OPTS) distnode.c

master_slave.o: distnode.h listnode.h message.h master_slave.c
	$(CC) $(CC_OPTS) master_slave.c

clean:
	@-rm *.o *.pipe master_slave
