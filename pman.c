/*
	PMan.c
	Basic process manager with support for pausing processes and viewing info.

	Jason Parker
	V00857251
	CSC360 - Assignment 1
 */

#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
#include <signal.h>

#include <readline/readline.h>
#include <readline/history.h>

#define DEBUG 1

#define MAX_LEN 35
#define RUNNING 1
#define STOPPED 0

const char* PROMPTSTRING = "PMan>: ";
char* COMMANDS[] = {
	"bg",
	"bgkill",
	"bgstart",
	"bgstop",
	"pstat",
	"bglist",
	"exit"
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

/* ### LIST FUNCTIONS ### */

//find a node in the tracked processes with a pid. Returns NULL if not found
node_t* findNode(pid_t pid) {
	node_t* curr = listHead;

	if(listHead == NULL) return NULL;

	while(curr->pid != pid) {
		if (curr->next == NULL) return NULL;
		curr = curr->next;
	}
	return curr;
}

// Append a process to the process list
void appendNode(pid_t pid, char* cmd) {
	node_t* new_proc = (node_t*)malloc(sizeof(node_t));
	new_proc->pid = pid;
	new_proc->cmd = cmd;
	new_proc->status = RUNNING;
	new_proc->next = NULL;
	new_proc->prev = listTail;

	if (listTail != NULL) listTail->next = new_proc;
	listTail = new_proc;
	if (listHead == NULL) listHead = new_proc;
}

// Remove the node of pid from the tracked process list
void removeNode(pid_t pid) {
	node_t* node = findNode(pid);
	if (node != NULL) {
		if (node == listHead) listHead = listHead->next;
		else node->prev->next = node->next;
		if (node == listTail) listTail = node->prev;
		else node->next->prev = node->prev;
	} else printf("ERR: Process %d does not exist.\n", pid);
}

//transform a string to a pid and return it. Return -1 if not valid.
// DOES NO CHECK IF PID EXISTS.
pid_t strToPid(char* s) {
	int i;
	for (i = 0; i < strlen(s); i++){
		if (!isdigit(s[i])) return -1;
	}
	return atoi(s);
}

//forks into a child process and attempts to execute the command given in args.
void bg(char** args, int argcount) {
	if (args) {
		if (!access(args[1], X_OK)) { // only fork if the command is actually executable.
			pid_t pid = fork(); // start a child
			if (pid == 0) { //hello child
				execvp(args[1], &args[1]);
				printf("ERR: failed to execute command %s\n", args[1]);
				exit(1);
			} else if (pid > 0) { // when back in the parent, add the started child to the list.
				printf("Process %d was started\n", pid);
				appendNode(pid, args[1]);
				usleep(1000); //take a nap for a second
			} else {
				printf("ERR: Could not fork :(");
			}
		} else {
			printf("ERR: can not execute %s\n", args[1]);
		}
	}
}

//These three functions are more or less identical - check if pid exists and send the relevant signal to them.
//Erros on bad pid or fail to send signal.
void bgkill(pid_t pid) {
	if (pid == -1) {
		printf("Error: SOMETHING\n");
		return;
	}
	node_t* node = findNode(pid);

	if(node == NULL) {
		printf("ERR: Process %d does not exist.\n", pid);
		return;
	}

	if (kill(pid, SIGTERM)) printf("ERR: failed to send SIGTERM to process %d\n", pid);
}

void bgstart(pid_t pid) {
	if (pid == -1) {
		printf("Error: SOMETHING\n");
		return;
	}
	node_t* node = findNode(pid);

	if(node == NULL) {
		printf("ERR: Process %d does not exist.\n", pid);
		return;
	}

	if (kill(pid, SIGCONT)) printf("ERR: failed to send SIGCONT to process %d\n", pid);
}

void bgstop(pid_t pid) {
	if (pid == -1) {
		printf("Error: SOMETHING\n");
		return;
	}
	node_t* node = findNode(pid);

	if(node == NULL) {
		printf("ERR: Process %d does not exist.\n", pid);
		return;
	}

	if (kill(pid, SIGSTOP)) printf("ERR: failed to send SIGSTOP to process %d\n", pid);
}

//prints out the stat file info for a pid. ONLY DOES THIS IF PID IS TRACKED BY pman
void pstat(pid_t pid) {
	node_t* node = findNode(pid);

	if (node == NULL) {
		printf("ERR: %d is not a pman tracked process or does not exist.\n", pid);
		return;
	}

	char* path;
	char* lb;

	char comm[];
	char state;
	unsigned long int utime;
	unsigned long int stime;
	unsigned long int rss;
	long int vcs;
	long int nvcs;

	sprintf(path, "/proc/%d/stat", pid);
	FILE* stat = fopen(path, "r");
	if (stat != NULL) {
		fscanf(stat, "* %s %c * * * * * * * * * * %lu %lu * * * * * * * * %ld", &comm, &state, &utime, &stime, &rss);
		fclose(stat);
	} else {
		printf("ERR: Could not open %s", path);
		return;
	}

	//stat+us
	strcat(path, "us");
	FILE* status = fopen(path, "r");
	char* statusarr[40];
	if (status != NULL) {
		int i = 0;
		for (; i < 32; i++) {
			if (fgets(statusarr[i], 40, status) == null) break;
		}
		vcs = statusarr[i - 2];
		nvcs = statusarr[i - 1];
		fclose(status);
	} else {
		printf("ERR: Could not open %s", path);
		return;
	}

	printf("pid\t: %d\n", pid);
	printf("comm\t: %s\n", comm);
	printf("state\t: %c\n", state);
	printf("utime\t: %lu\n", utime);
	printf("stime\t: %lu\n", stime);
	printf("rss\t: %lu\n", rss);
	printf("voluntary_ctx_switches\t: %ld\n", vcs);
	printf("nonvoluntary_ctx_switches\t: %ld\n", nvcs);
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

// sift throug user input of commands to pman. handles rough input validation,
// but does not safety check actual arguments, that must be handled by the bg#### functions.
void execute(char* args[], int argcount) {
	//is the first argument passed a valid command? if so which one!
	int i;
	int cmd = -1;
	for (i = 0; i < 7; i++) {
		if (!strcmp(args[0], COMMANDS[i])) {
			cmd = i;
			break;
		}
	}
	if (cmd == 7) {
		printf("PMan: Good-Bye!\n");
		exit(0);
	} else if (cmd == -1) {
		printf("PMan: command %s not found\n", args[0]);
		return;
	} else if (cmd != 5 && argcount < 2) {
		printf("ERR: Not enough arguments supplied for %s\n", args[0]);
		return;
	}

	switch (cmd) { // handle the "generic cases"
		case 0: {
			bg(args, argcount);
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
			break;
		}
	}
}

// use waitpid to determine the state change of child processes. Update as necessary.
void updateBackgroundProcess() {
	pid_t pid;
	int stat;
	while (true) {
		pid = waitpid(-1, &stat, WCONTINUED | WNOHANG | WUNTRACED);
		if (pid > 0) {
			if (WIFEXITED(stat)) {
				printf("Background process %d terminated\n", pid);
				removeNode(pid);
			} else if (WIFSTOPPED(stat)) {
				printf("Background process %d was stopped\n", pid);
				findNode(pid)->status = STOPPED;
			} else if (WIFSIGNALED(stat)) {
				printf("Background process %d was killed\n", pid);
				removeNode(pid);
			} else if (WIFEXITED(stat)) {
				printf("Background process %d started\n", pid);
				findNode(pid)->status = RUNNING;
			}
		} else {
			break;
		}
	}
}

// build the prompt for user input and runs the main program loop.
int main() {

	int argcount, i;
	while (true) {
		updateBackgroundProcess();
		char* args[MAX_LEN];
		char* token;
		argcount = 0;

		char* prompt = readline("PMan:> ");
		token = strtok(prompt, " ");
		if(strcmp(token, "")) {
			for (i = 0; i < MAX_LEN; i++) {
				if (token) argcount++;
				args[i] = token;
				token = strtok(NULL, " ");
			}
		}
		execute(args, argcount);
		updateBackgroundProcess();
	}
}
