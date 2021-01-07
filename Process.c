/**
 * Process.c
 *
 * Author - Thomas Wilkinson
 * Author - Na Li
 * 
 * Description: Contains createProcess() which creates the process struct that represents each process
 */

#define _GNU_SOURCE
#include "Process.h"
#include "PageTable.h"
#include <stdbool.h>
#include <stdlib.h>
#include <search.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>


/**
 * createProcess() creates the process struct, setting fields to null and 0
 * 
 * @return - the process struct
 */
struct Process *createProcess(){
	
	struct Process *newProcess = (struct Process *)malloc(sizeof(struct Process));
	if(newProcess == NULL){	
		fprintf(stderr, "Error: malloc failed.\n");
	}
	newProcess->thePageTable = createPageTable();
	newProcess->PID = NULL;
	newProcess->startPosition = 0;
	newProcess->endPosition = 0;	
	newProcess->blocked = false;
	newProcess->memoryReads = 0;
	return newProcess;

}
