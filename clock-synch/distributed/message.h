#ifndef MESSAGE_H
#define MESSAGE_H

#include <time.h>
#include "listnode.h"
/*
  This will let us do things like MessageType type = MSG_KILL;
  An enum also looks better than a bunch of defines and makes our
  a little more readable. (maybe)
*/
typedef enum message_types{
  MSG_KILL,
  MSG_TICK,
  MSG_GETTIME,
  MSG_SETTIME,
  MSG_INIT_RING,
  MSG_ELECTION_E,
  MSG_ELECTION_C,


  //responses
  MSG_404,
  MSG_GETTIME_R,

  //messages sent by the driver to print system status
  CMD_KILL,
  CMD_REV,
  CMD_TIME,
  CMD_STAT,
  CMD_EXIT
} MessageType;

/*
  define the layout of the header for messages we pass
*/
typedef struct header{
	MessageType type;
	int source_pid;
	int timestamp;
	int body_length;      //length of body that is appended to the header
  int metadata;         //what the 4 bytes are is determined by MessageType
} MessageHeader;

void printMessageHeader(MessageHeader *);

#endif
