#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "distnode.h"

#define CREAT_FLAGS (S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH)

#define RUN_TIME 600 //seconds
#define BUF_SIZE 1024
#define TIMEOUT 20000
#define ADJ_TIME 5000 //ms
#define MIN_HB (ADJ_TIME - 3000)
#define MAX_HB (ADJ_TIME + 3000)

extern int errno;

//private interface
void adjustTime(int, DistNode *);
void interpretMessage(char *, DistNode *); 
void killDistNode(DistNode *);
char* convertPidToPipe(int);

//constructor
DistNode *initializeDistNode(int pid, float drift){
	DistNode *node = malloc(sizeof(DistNode));
	char* pipe;
	pipe = convertPidToPipe(pid);

	// Create pipe
	if ((mkfifo(pipe, CREAT_FLAGS) == -1) && errno != EEXIST) {
		perror("Unable to create pipe");
		exit(-1);
	}
  
	node->pid = pid;
	node->master_pid = 0;
	node->alive = 1;
	node->running = 1;
	node->initiated_election = 0;
	node->election_in_progress = 0;
	node->active_drift = drift;
	node->default_drift = drift;
	node->metadata = 0;

	// Query UTC Time
	time(&node->start_time);

	// Create local history
	node->local_hist = 0;

	return node;
}

void printDistNodeTime(DistNode *node){
  printf("\nDistNode: %d\n"
    "start time: %d\noffset: %dms\n", 
    node->pid, (int)node->start_time, node->offset
  );
}

void printDistNode(DistNode *node){
  printf("\nDistNode: %d\n"
    "\talive: %d\n"
    "\trunning: %d\n"
    "\tlast sent: %d\n"
    "\toffset: %d\n"
    "\tactive drift: %f\n"
    "\tdefault drift: %f\n"
    "\thistory: %s\n",
    node->pid, node->alive, node->running, node->last_sent, node->offset,
    node->active_drift, node->default_drift, serializeListNode(node->local_hist)
  );
}

void killDistNode(DistNode *node){
}

void startDistNode(DistNode *node){
	char header_buf[sizeof(MessageHeader)], msg_buf[BUF_SIZE], total_buf[BUF_SIZE + sizeof(MessageHeader)];
	int fd, header_bytes, msg_bytes, offset = 0, last_hb = 0, hb_length = 0;
	MessageHeader* head;
	char* pipe_name = convertPidToPipe(node->pid);

	// Open pipe for reading
	if ((fd = open(pipe_name, O_RDONLY)) == -1) {
		perror("Failed to open pipe for reading");
		exit(-1);
	}
	srand(time(NULL));

	while (node->running) {

    //if master node, check if it needs to send a heartbeat
		if( node->pid == node->master_pid) {
			if((node->offset - last_hb) > hb_length) {
				hb_length = MIN_HB + (rand() % (MAX_HB - MIN_HB + 1));
				last_hb = node->offset;
				int sent = 0;
				//ask every node for their time
				ListNode *iter = node->local_hist;
				while(iter){
					node->msg_count++;sendMessage(iter->pid, MSG_GETTIME, 0, 0);
					++sent;
					iter = iter->next;
				}

				node->resp_count = sent;
				node->max_resp = sent;
			}
		}
		// Check to see if master has died.  Determined to be dead if 5 seconds passed and there is no communication from master
		//else if (node->offset - node->master_comm > TIMEOUT) {
		if ((node->election_in_progress == 0) && (node->offset - node->master_comm > TIMEOUT)) {
			printf("Election Started\n");
      // Start election algorithm
			node->last_sent = MSG_ELECTION_E;
			node->master_pid = 0;
			node->initiated_election = 1;
			node->election_in_progress = 1;
			node->msg_count++;sendMessage(node->local_hist->pid, MSG_ELECTION_E, node->pid, serializeListNode(node->local_hist));
		}
  
		//read from pipe for new messages
		//if ((num_bytes = read(fd, buf, sizeof(MessageHeader) + BUF_SIZE)) == -1) {
		while ((header_bytes = read(fd, header_buf, sizeof(MessageHeader))) > 0) {
			head = (MessageHeader*) header_buf;
			if ((msg_bytes = read(fd, msg_buf, head->body_length)) == -1) {
				perror("Unable to read from pipe");
				exit(-1);
			}

			memcpy(total_buf, header_buf, sizeof(MessageHeader));
			memcpy(total_buf + sizeof(MessageHeader), msg_buf, strlen(msg_buf) + 1);
			interpretMessage(total_buf, node);
		}
		if (header_bytes == -1) {
			perror("Unable to read from pipe");
			exit(-1);
		}
	}

	// Close pipe
	if (close(fd) == -1) {
		perror("Unable to close pipe");
		exit(-1);
	}
}

void exitDistNode(DistNode* node) {

}

void interpretMessage(char* msg, DistNode* node) {
	MessageHeader *packet = (MessageHeader *)msg;
	if (packet->type != MSG_TICK)
		printMessageHeader(packet);
	
  // Copy the packet_history to your local_history if message does not come from driver
	if (packet->type != MSG_TICK) {
		//TODO, we should find someway to check if the body is actually serialized data
		if(packet->body_length > 1){
		    free(node->local_hist);
		    node->local_hist = deserializeListNode(msg + sizeof(MessageHeader));
		}
   }
  
  //record last time node receives a message from the master
  if( packet->source_pid ==  node->master_pid && packet->type != MSG_404) 
    node->master_comm = node->offset;  
  
  int true_offset = 0;
  
  //only respond to inter node messages if this node is alive
  if(!node->alive) {
    if(findListNode(node->local_hist, packet->source_pid) && packet->source_pid != node->pid){
      node->msg_count++;sendMessage(packet->source_pid, MSG_404, packet->metadata, 0);
    }
  }
  else{
    //check message type and then process it appropriately
    
    // used to write final node stats to file
    FILE *fp;
    char *filename;
    struct tm *ptm;
    time_t stop_time;

    switch (packet->type) {
      case MSG_TICK:
        if( time(NULL) - node->start_time  >= RUN_TIME){
          filename = malloc(64 * sizeof(char));
          sprintf(filename, "%d", (int)node->start_time);
          strcat(filename, ".csv");
          stop_time = node->start_time + (node->offset / 1000);
          ptm = gmtime(&stop_time);

          while((fp = fopen(filename, "a")) == NULL);
			    fprintf(fp, "%i,%i,%i,%i\n", getpid(), (int)stop_time, node->msg_count, (int)time(NULL));
          fclose(fp);
          printf("Node %d ending. current time: %s\n", getpid(), asctime(ptm));

          node->running = 0;

        }
        else{
          node->offset += node->active_drift * 1000;
        
          //reset drift
          if (node->offset - node->prev_offset >= ADJ_TIME){
            node->active_drift = node->default_drift;
            node->prev_offset = node->offset;
          }
        }
        break;
      case MSG_INIT_RING:
        while (node->local_hist->pid != node->pid) {
          enqueue(&(node->local_hist), dequeue(&(node->local_hist)));
        }
        enqueue(&(node->local_hist), dequeue(&(node->local_hist)));
        break;
      case MSG_ELECTION_E:
		// Move self to bottom of list
		if (packet->type != MSG_INIT_RING) {
			enqueue(&(node->local_hist), dequeue(&(node->local_hist)));
		}
        //packet->metadata containst PID of node that initiated election
        // If you receive E, and you haven't started an election, forward it
		node->master_pid = 0;
        if (node->initiated_election == 0) {
		  node->last_sent = MSG_ELECTION_E;
          node->msg_count++;sendMessage(node->local_hist->pid, MSG_ELECTION_E, packet->metadata, serializeListNode(node->local_hist));
        }
        // Else if you have started an election, determine if the source of this election is you
        // If it is, you are the coordinator.  Send C
        else if (packet->metadata == node->pid) {	
		  node->last_sent = MSG_ELECTION_C;
          node->msg_count++;sendMessage(node->local_hist->pid, MSG_ELECTION_C, packet->metadata, serializeListNode(node->local_hist));
        }
        // If it is not, only forward the message E if it's source pid is higher than yours
        else if (packet->metadata > node->pid) {
		  node->last_sent = MSG_ELECTION_E;
          node->msg_count++;sendMessage(node->local_hist->pid, MSG_ELECTION_E, packet->metadata, serializeListNode(node->local_hist));
        }
        else {
          // Else drop the election message
          printf("Node (%i) dropping msg inititated by %i because it lost the tiebreaker\n", node->pid, packet->metadata);
        }

        node->election_in_progress = 1;
        break;
      case MSG_ELECTION_C:
		// Move self to bottom of list
		if (packet->type != MSG_INIT_RING) {
			enqueue(&(node->local_hist), dequeue(&(node->local_hist)));
		}
        // If you receive C, set your coordinator to C and forward the message
        // If you receive C and you are the coordinator, drop it
        // metadata contains PID of node that started election
        if (packet->metadata == node->pid) {
            printf("Node (%i) has been elected master\n", node->pid);
        }
        else{	
		  node->last_sent = MSG_ELECTION_C;
          node->msg_count++;sendMessage(node->local_hist->pid, MSG_ELECTION_C, packet->metadata, serializeListNode(node->local_hist));
        }

        node->master_pid = packet->metadata;
        node->initiated_election = 0;
        break;
      case MSG_GETTIME:
        node->election_in_progress = 0;
        node->msg_count++;sendMessage(packet->source_pid, MSG_GETTIME_R, node->offset, 0);
        break;
      case MSG_SETTIME:
        //calculate new drift value
        true_offset = packet->metadata;
        node->active_drift += (float)(true_offset - node->offset) / ADJ_TIME;
        if( node->active_drift <= 0 )
          node->active_drift = .5;
        break;
      case MSG_EXIT:
        node->running = 0;
        break;

      //response messages
      case MSG_404:
        if( node->pid != node->master_pid) {
          if (packet->source_pid != node->master_pid) {
             dequeue(&node->local_hist);
             node->msg_count++;sendMessage(node->local_hist->pid, node->last_sent, packet->metadata, serializeListNode(node->local_hist));
          }
        }
        else{ //master
          if( --node->resp_count < 0 ){
            node->resp_count = 0;
            --node->max_resp;
          }

          ListNode *temp = findListNode(node->local_hist, packet->source_pid);
          removeListNode(&(node->local_hist),temp, 0);
        }
        break;
      case MSG_GETTIME_R:
        --node->resp_count;
        //hold sum of offsets used to calculate avg offset
        node->metadata += packet->metadata;

        if(node->resp_count == 0 ){
          //only broadcast systemtime if master
          if( node->pid == node->master_pid) {
            //all responses received, calculate system time
            //metadata holds average system time
            node->metadata = (node->metadata) / (node->max_resp);
          
            //send all nodes the average system time
            ListNode *iter = node->local_hist;
            while(iter){
              node->msg_count++;sendMessage(iter->pid, MSG_SETTIME, node->metadata, 0);
              iter = iter->next;
            }
            node->metadata = 0;
          }
        }
        break;
      default:
        break;
    }
  } 

  //driver commands, these always go through even if node is dead
  switch(packet->type){
      case CMD_STAT:
        printDistNode(node);
        break;
      case CMD_TIME:
        printDistNodeTime(node);
        break;
      case CMD_KILL:
        node->alive = 0;
        break;
      case CMD_REV:
        node->alive = 1;
        break;
		default:
			break;
	}
}

int sendMessage(int dest_pid, char msg_type, int metadata, char *body){
  int fd;
	char* pipe;
  
  int body_len = (body) ? strlen(body) : 0;
	int pktsize = body_len + sizeof(MessageHeader) + 1;
	char *packet = malloc(pktsize * sizeof(char));
	MessageHeader *header = (MessageHeader *)packet;
	header->source_pid = getpid();
	header->type = msg_type;
	header->body_length = body_len + 1;
	header->timestamp = 0;
	header->metadata = metadata;
  
  if(body)
	  memcpy(packet + sizeof(MessageHeader), body, strlen(body));
	packet[pktsize - 1] = 0;

	pipe = convertPidToPipe(dest_pid);

	// Open pipe
	if ((fd = open(pipe, O_WRONLY)) == -1) {
		printf("Node (%i) ", getpid());
		perror("Failed to open pipe for writing");
		printf("%s\n", pipe);
		return -1;
	}
	
	if (write(fd, packet, pktsize) == -1) {
		perror("Failed to write to pipe");
		return -1;
	}
	// Close pipe
	if (close(fd) == -1) {
		perror("Failed to close pipe");
		return -1;
	}
	return 0;
}

char* convertPidToPipe(int pid) {
	char* pipe;

	// Allocate memory
	if ((pipe = (char*) malloc(sizeof(char) * 11)) == NULL) {
		perror("Failed to malloc");
		exit(-1);
	}

	// Convert pid to string
	if (sprintf(pipe, "%d", pid) < 0) {
		perror("Failed to convert pid to char");
		exit(-1);
	}

	if (strcat(pipe, ".pipe") == NULL) {
		perror("Failed to strcat");
		exit(-1);
	}
	
	return pipe;
}
