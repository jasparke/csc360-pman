/*
	PMan.c
	Basic process manager with support for pausing processes and viewing info.

	Jason Parker
	V00857251
	CSC360 - Assignment 1
 */

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <malloc.h>
#include <sys/types.h>
#include <signal.h>

#include <readline/readline.h>
#include <readline/history.h>

enum {
	MAX_LEN = 120,
	RUNNING = 1,
	STOPPED = 0
};

const char* PROMPTSTRING = "PMan>: ";
char* COMMANDS[] = {
	"bg",
	"bgkill",
	"bgstart",
	"bgstop",
	"pstat",
	"bglist"
};

//node_t for double linked list of cmds.
typedef struct node_t {
	pid_t pid;
	int status;
	char* cmd;
	struct node_t* next;
	struct node_t* prev;
} node_t;

node_t* listHead = NULL;
node_t* listTail = NULL;


//forks into a child process and attempts to execute the command given in args.
void bg(char** args) {
	pid_t pid = fork();
	if (pid == 0) { //hello child
		execvp(args[1], &args[1]);
		printf("ERR: failed to execute %s\n", args[1]);
		exit(1);
	} else if (pid > 0) {
		printf("Process %d was started\n", pid);
		appendNode(pid, args[1]);
		usleep(1000);
	} else {
		printf("ERR: Could not fork() :(");
	}
}

//These three functions are more or less identical - check if pid exists and send the relevant signal to them.
//Erros on bad pid or fail to send signal.
void bgkill(pid_t pid) {
	node_t* node = findNode(pid);

	if(node == NULL) {
		printf("ERR: Process %d does not exist.\n", pid);
		return;
	}

	if (kill(pid, SIGTERM)) printf("ERR: failed to send SIGTERM to process %d\n", pid);
}

void bgstart(pid_t pid) {
	node_t* node = findNode(pid);

	if(node == NULL) {
		printf("ERR: Process %d does not exist.\n", pid);
		return;
	}

	if (kill(pid, SIGCONT)) printf("ERR: failed to send SIGCONT to process %d\n", pid);
}

void bgstop(pid_t pid) {
	node_t* node = findNode(pid);

	if(node == NULL) {
		printf("ERR: Process %d does not exist.\n", pid);
		return;
	}

	if (kill(pid, SIGSTOP)) printf("ERR: failed to send SIGSTOP to process %d\n", pid);
}

void pstat(pid_t pid) {
	printf("you didn't do this yet\n");
}

//prints out a list of all jobs with their pid, and the total count of jobs.
void bglist() {
	int count = 0;
	node_t* curr = listHead;

	while (curr != NULL) {
		printf("%d:\t %s %s\n",curr->pid, curr->cmd, (curr->status == STOPPED)? "(stopped)" : "(running)");
		count++;
		curr = curr->next;
	}
	printf("Total background jobs: \t %d\n", count);
}

/* ### LIST FUNCTIONS ### */

// Append a process to the process list
void appendNode(pid_t pid, char* cmd) {
	node_t* new_proc = (node_t*)malloc(sizeof(node_t));
	new_proc->pid = pid;
	new_proc->cmd = cmd;
	new_proc->status = RUNNING;
	new_proc->next = NULL;
	new_proc->prev = listTail;

	listTail = new_proc;
	if (listHead == NULL) listHead = new_proc;
}

// Remove the node of pid from the tracked process list
void removeNode(pid_t pid) {
	node_t* node = findNode(pid);

	if (node != NULL) {
		if (node == listHead) listHead = node->next;
		if (node == listTail) listTail = node->prev;
		node->next->prev = node->prev;
		node->prev->next = node->next;
	} else printf("ERR: Process %d does not exist.\n", pid);
}

//find a node in the tracked processes with a pid. Returns NULL if not found
node_t* findNode(pid_t pid) {
	node_t* curr = listHead;
	while (curr != NULL) {
		if (curr->pid == pid) break;
	}

	return curr;
}



//transform a string to a pid and return it. Return -1 if not valid.
// DOES NO CHECK IF PID EXISTS.
pid_t strToPid(char* s) {
	for (int i = 0; i < strlen(s); i++)
		if (!isdigit(s.charat(i))) return -1;

	return atoi(s);
}

// attempt to execute a command if it has valid arguments, otherwise print the error.
void execute(int cmd, char** args) {
	switch (cmd) { // handle the "generic cases"
		case 0: {
			bg(args);
			break;
		}

		case 1: {
			bgkill(strToPid(args[1]));
			break;
		}

		case 2: {
			bgstart(strToPid(args[1]));
			break;
		}

		case 3: {
			bgstop(strToPid(args[1]));
			break;
		}

		case 4: {
			pstat(strToPid(args[1]));
			break;
		}
		case 5: {
			bglist();
			break;
		}
		default: {
			printf("PMan:> %s:\t command not found", args[0]);
			break;
		}
	}
}

// build the prompt for user input and runs the main program loop.
int main() {
	while(true) {
		char* prompt[MAX_LEN] = NULL;

		updateBackgroundProcess();
		prompt = readline("PMan:> ");
		if (strcmp(prompt, "")) {
			char* tokenizedPrompt = strtok(prompt, " ");
			int command = -1;
			for (int i = 0; i < 6; i++) {
				if (strcmp(tokenizedPrompt[0], COMMANDS[i]) == 0) {
					command = i;
					break;
				}
			}
			execute(command, tokenizedPrompt);
		}
		updateBackgroundProcess();
	}

	 return 0;
}
