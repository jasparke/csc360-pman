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
#include <sys/types.h>
#include <readline/readline.h>

#define MAX_INPUT_LEN 80


void bg(char* process) {

}

void bgkill(int pid) {

}

void bgstart(int pid) {

}

void bgstop(int pid) {

}

void bglist() {

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

/**
 * read the input from user and make sure its valid
 * @param input
 * @return true if no issue, false otherwise
 */

/**
 * build the prompt for user input
 * @return n/a
 */
int main() {
	 while(true) {
		 char* input[MAX_INPUT_LEN];

		 updateProcess();
		 if (readInput(input) {
			 updateFromInput(input);
		 }
		 updateProcess();
	 }

	 return 0;
}
