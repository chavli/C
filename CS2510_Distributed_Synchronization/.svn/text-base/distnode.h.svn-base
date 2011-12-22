#ifndef DISTNODE_H
#define DISTNODE_H

#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include "message.h"

typedef struct dnode{
	int pid;
	int master_pid;
	time_t start_time;
	MessageType last_sent;
	int master_comm;
	int offset, prev_offset; //ms
	int msg_count; //ms
	float active_drift, default_drift;
	ListNode *local_hist;
  short int resp_count, max_resp;
  int metadata;                 //extra 4 bytes for whatever

  //bit fields for booleans
  unsigned char alive:1;
	unsigned char running:1;
	unsigned char initiated_election:1;
	unsigned char election_in_progress:1;
  unsigned char padding:4;               //you can use these 4 bits for anything
} DistNode;

DistNode *initializeDistNode(int, float);
void printDistNode(DistNode *);
void printDistNodeTime(DistNode *);

void startDistNode(DistNode *);
void exitDistNode(DistNode *);

int sendMessage(int dest_pid, char msg_type, int metadata, char *body);
#endif
