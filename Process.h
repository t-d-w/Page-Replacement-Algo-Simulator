/**
 * Process.h
 *
 * Author - Thomas Wilkinson
 * CS Login: twilkinson
 * Author - Na Li
 * CS Login: na
 * 
 * Description: Contains createProcess() which creates the process struct that represents each process
 */

#ifndef PROCESS_H
#define PROCESS_H

#include <stdbool.h>
#include <stdio.h>
#include <search.h>
#include <string.h>
#include <stdlib.h>

/**
 * Process struct represents each process, storing start and end position, pid, reads, and its page table tree 
 */
struct Process{
	char *PID;
	long int startPosition;
	long int endPosition;
	bool blocked;	
	int memoryReads;
	struct PageTable *thePageTable;
};

/**
 * createProcess() creates the process struct, setting fields to null and 0
 * 
 * @return - the process struct
 */
struct Process *createProcess();

#endif

