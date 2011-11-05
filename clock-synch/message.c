#include <stdlib.h>
#include <stdio.h>
#include "message.h"

void printMessageHeader(MessageHeader *header){
  printf("Node (%i): ", getpid());
  printf("source: %d", header->source_pid);
  printf(", timestamp: %d", header->timestamp);
  printf(", metadata: %d", header->metadata);
  printf(", body_length: %d", header->body_length);
  printf(", type: ");
  switch (header->type) {
    case MSG_KILL:
      printf("MSG_KILL\n");
      break;
    case MSG_TICK:
      printf("MSG_TICK\n");
      break;
    case MSG_GETTIME:
      printf("MSG_GETTIME\n");
      break;
    case MSG_GETTIME_R:
      printf("MSG_GETTIME_R\n");
      break;
    case MSG_SETTIME:
      printf("MSG_SETTIME\n");
      break;
    case MSG_INIT_RING:
      printf("MSG_INIT_RING\n");
      break;
    case MSG_ELECTION_E:
      printf("MSG_ELECTION_E\n");
      break;
    case MSG_ELECTION_C:
      printf("MSG_ELECTION_C\n");
      break;
    case MSG_404:
      printf("MSG_404\n");
      break;
    case CMD_TIME:
      printf("CMD_TIME\n");
      break;
    case CMD_STAT:
      printf("CMD_STAT\n");
      break;
    case CMD_KILL:
      printf("CMD_KILL\n");
      break;
    case CMD_REV:
      printf("CMD_REV\n");
      break;
    case CMD_EXIT:
      printf("CMD_EXIT\n");
      break;
    default:
      printf("NO TYPE\n");
      break;
  }    	
}
