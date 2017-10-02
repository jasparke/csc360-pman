/*
	pman.c
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
#include <readline/readline.h>
#include <readline/history.h>


#define MAX_LEN 120
#define RUNNING 1
#define STOPPED 0

typedef struct node_t {
	pid_t pid;
	int status;
	char* proc;
	struct node_t* next;
	struct node_t* prev;
} node_t;

node_t* listHead = NULL;
node_t* listTail = NULL;

void bg(char* process) {

}

void bgkill(pid_t pid) {

}

void bgstart(pid_t pid) {

}

void bgstop(pid_t pid) {

}

void pstat(pid_t pid) {

}

//prints out a list of all jobs with their pid, and the total count of jobs.
void bglist() {
	int count = 0;
	node_t* curr = listHead;

	while (curr != NULL) {
		printf("%d:\t %s %s\n",curr->pid, curr->proc, (curr->status == STOPPED)? "(stopped)" : "(running)");
		count++;
		curr = curr->next;
	}
	printf("Total background jobs: \t %d", count);
}

void updateProcess() {
	printf("updateprocess() called.");
}

int readInput(char** input) {


	printf("readInput() called.");
	return true;
}

void updateFromInput(input) {
	printf("updateFromInput() called.");
}


/* ### LIST FUNCTIONS ### */

// Append a process to the process list
void appendNode(pid_t pid, char* proc) {
	node_t* new_proc = (node_t*)malloc(sizeof(node_t));
	new_proc->pid = pid;
	new_proc->proc = proc;
	new_proc->status = RUNNING;
	new_proc->next = NULL;
	new_proc->prev = listTail;

	listTail = new_proc;
	if (listHead == NULL) { listHead = new_proc; }
}

void removeNode(pid_t pid) {
	node_t* curr = listHead;

	while (curr != NULL) {
		if (curr->pid == pid) {
			if (curr == listHead) listHead = curr->next;
			if (curr == listTail) listTail = curr->prev;
			curr->next->prev = curr->prev;
			curr->prev->next = curr->next;
			return;
		}
		curr = curr->next;
	}
	printf("ERR: Process %d does not exist.", pid);
}

/**
 * read the input from user and make sure its valid
 * @param input
 * @return true if no issue, false otherwise
 */

// build the prompt for user input and runs the main program loop.
int main() {
	 while(true) {
		 char* input[MAX_LEN];

		 updateProcess();
		 if (readInput(input) {
			 updateFromInput(input);
		 }
		 updateProcess();
	 }

	 return 0;
}
