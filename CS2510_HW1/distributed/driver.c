#include <assert.h>
#include <math.h>
#include "distnode.h"

#define MAX_TIME 10	//seconds

int main(int argc, char** argv)
{
	int num_procs, num_forked, fd, i;
	time_t start = time(NULL);
	pid_t pid;
	float drift;
	char* msg, *procID;
	char** pids;
	char buf[100];
	ListNode* history;
	DistNode* dnode;
	FILE* fp;
    
	if (argc != 2) {
		fprintf(stderr, "Usage: %s: Must provide number of processes\n", *argv);
		exit(1);
	}
	   else if ((num_procs = atoi(*(argv + 1))) < 0) {
	   fprintf(stderr, "Usage: %s: Number of children must be non-negative\n", *argv);
	   exit(1);
	}

	char* filename = malloc(30);
	sprintf(filename, "%d", (int)start);

	// Seed randum number generator
	srand(time(NULL));

	// The parent process simply needs to keep a list of process IDs.  These can be converted to char* which are the fifo names
	// Allocate memory for pids list and procID (max length for process ID is 5 characters (need one for '\0'
    if (((pids = (char**) malloc(sizeof(char*) * num_procs)) == NULL) || ((procID = (char*) malloc(sizeof(char) * 6)) == NULL))
	{
		perror("Failed to malloc");
        exit(-1);
	}

	// Allocate memory for history
    if ((history = (ListNode*) malloc(sizeof(ListNode))) == NULL)
	{
		perror("Failed to malloc");
        exit(-1);
	}
	history = 0;

	// Allocate memory for each entry in pids
	for (i = 0; i < num_procs; ++i) {
		if ((*(pids + i) = (char*) malloc(sizeof(char) * 6)) == NULL) {
			perror("Failed to malloc");
			exit(-1);
		}
	}

	num_forked = 0;
	while (num_forked < num_procs) {
		drift = 0.5;
		drift += ((rand() % 11) / 10.0);

		if ((pid = fork()) < 0) {
			perror("Failed to fork");
			exit(-1);
		}

	    if (pid == 0) {
        // Child initializes self (which creates a fifo) and waits for communication
		    startDistNode(initializeDistNode(getpid(), drift, start));
        exit(0);
		}
		else {
			// Parent needs to remember the id of the child process
			// Convert the processID to a string
			if (sprintf(procID, "%d", pid) < 0) {
				perror("Failed to convert pid to char*");
				exit(-1);
			}

			// Iterate through pids with a counter (num_forked)
			if (strcpy(*(pids + num_forked), procID) == NULL) {
				perror("Failed to strcpy");
				exit(-1);
			}

			// Add to history as well
			enqueue(&history, initializeListNode(atoi(procID)));
		}

		++num_forked;
	}

	// Fork one more process to send "ticks" to nodes
	if ((pid = fork()) < 0) {
		perror("Failed to fork");
		exit(-1);
	}

	if (pid > 0) {
		sleep(1);

		// Send initial history message to every node
		for (i = 0; i < num_procs; ++i) 
			sendMessage(atoi(*(pids + i)), MSG_INIT_RING, 0,serializeListNode(history));

		// Send ticks
		while (1) {
			sleep(1);
		  // Send tick to nodes
			for (i = 0; i < num_procs; ++i)
				sendMessage(atoi(*(pids + i)), MSG_TICK, 0, 0);
		}

		while(wait(NULL));

		fp = fopen(filename, "r");
		
		char line[256];
		char time[32];
		char trash[224];
		int systime, lines;	
		while(fgets(line, 256, fp)){
			sscanf(line, "%s, %s", time, trash);
			systime += atoi(time);
			lines++;
		}
		fclose(fp);	
		systime /= lines;
		printf("Average System Time: %i\n", systime);	
	}

	//the fancy CLI
	char line[32];
  char *iter;
	printf("===== Clock Synchronizer =====\n");
  printf("type 'help' for help.\n");
	while(1) {
		printf("Command>>");
	  fgets(line, 32 ,stdin);
    //check if there actually is a command size == 1 means only \n
    if(strlen(line) > 1){
      line[strlen(line) - 1] = 0;
      char *command = strtok_r(line, " ", &iter);
      if( strcmp(command, "exit") == 0){
        for (i = 0; i < num_procs; ++i)
          sendMessage(atoi(*(pids + i)), MSG_EXIT, 0, 0);
        break;
      }
      else if( strcmp(command, "help") == 0){
        printf("\tkill <pid> - kill the node with pid\n");
        printf("\trevive <pid> - revive the node with pid\n");
        printf("\tstat <pid> - print status of a node\n");
        printf("\ttime <pid> - print time data of a node\n");
        printf("\tlist - print all pids\n");
        printf("\thelp - display this message\n");
        printf("\texit - stop all nodes and exit program\n");  
      }
      else if( strcmp(command, "list") == 0){
        printf("system nodes:\n");
        for(i = 0; i < num_procs; ++i)
          printf("\t%s\n", *(pids + i));
      }
      else if( strcmp(command, "stat") == 0){
        char *arg = strtok_r(NULL, " ", &iter);
        if(arg != NULL){
          if( sendMessage(atoi(arg), CMD_STAT, 0, 0) == -1 )
            printf("invalid pid: %s\n", arg);
        }
        else
          printf("usage: status <pid>\n");
      }
      else if( strcmp(command, "time") == 0){
        char *arg = strtok_r(NULL, " ", &iter);
        if(arg != NULL){
          if( sendMessage(atoi(arg), CMD_TIME, 0, 0) == -1 )
            printf("invalid pid: %s\n", arg);
        }
        else
          printf("usage: time <pid>\n");
      }
      else if( strcmp(command, "kill") == 0){
        char *arg = strtok_r(NULL, " ", &iter);
        if(arg){
          if( sendMessage(atoi(arg), CMD_KILL, 0, 0) == -1 )
            printf("invalid pid: %s\n", arg);
        }
        else
          printf("usage: kill <pid>\n");
      }
      else if( strcmp(command, "revive") == 0){
        char *arg = strtok_r(NULL, " ", &iter);
        if(arg){
          if( sendMessage(atoi(arg), CMD_REV, 0, 0) == -1 )
            printf("invalid pid: %s\n", arg);
        }
        else
          printf("usage: revive <pid>\n");
      }
      else{
        printf("unrecognized command: %s\n", command);
      }
     }
	}

	// Free all alloc'd memory
	free(procID);
	free(pids);

	exit(0);
}
