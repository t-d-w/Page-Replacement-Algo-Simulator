/** 
 * TraceParser.c
 *
 * Author - Thomas Wilkinson
 * Author - Na Li
 *
 * Description: The module handles readings of the tracefile and interact with other files in the project to simulate the page replacement algorithm.
 */

#define _GNU_SOURCE
#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <unistd.h>
#include "Process.h"
#include "DiskQueue.h"
#include "InvertedPageTable.h"
#include "PageTable.h"
#include "PageReplacementAlgorithm.h"
#include "Statistics.h"

/**
 * parseLineForNums takes in an individual line, and parses it for the pid, vpn pair. It also checks to make sure they 
 * are both made of digits and valid numbers.
 *
 * @return - char ** - the array which contains pid in spot 0 and vpn in spot 1
 * @param - currentLine - the current line to parse
 */
char **parseLineForNums(char *currentLine){

	char **thisTraceNums = malloc(sizeof(char *) * 2);
	for(int i = 0; i < 2; i++){
		thisTraceNums[i] = malloc(sizeof(char) * 15);
	}
	
	char *pid = malloc(sizeof(char) * 15);
	if(pid == NULL){
		fprintf(stderr, "Error: malloc failed.\n");
	}
	char *vpn = malloc(sizeof(char) * 15);	
	if(vpn == NULL){
		fprintf(stderr, "Error: malloc failed.\n");
	}

	char *stringToken = malloc(sizeof(char) * 50);

	stringToken = strtok(currentLine, " ");
	
	int count = 0;
	while(stringToken != NULL && count < 2){
		if(count == 0){

			strcpy(pid, stringToken);
		} else {
			strcpy(vpn, stringToken);
		}

		stringToken = strtok(NULL," ");
		count++;
	}
	free(stringToken);
	
	// check the pid chars
	int index1 = 0;
	char pidChar = pid[index1];
	while(pidChar != '\0' && !pidChar){
		if(isdigit(pidChar) == 0){
			// non decimal digit
			fprintf(stderr, "Error: Invalid PID\n");
			exit(1);
		}
		index1++;
		pidChar = pid[index1];
	}
	strcpy(thisTraceNums[0], pid);
	free(pid);



	// check the vpn chars
	int index2 = 0;
	char vpnChar = vpn[index2];
	while(vpnChar != '\0' && !vpnChar){
		if(isdigit(vpnChar) == 0){
			// non decimal digit
			fprintf(stderr, "Error: Invalid VPN, illegal character %c\n", vpnChar);
			exit(1);
		}
		index2++;
		vpnChar = vpn[index2];
	}
	strcpy(thisTraceNums[1], vpn);
	free(vpn);	

	return thisTraceNums;
}



/**
 * firstTrace does the initial trace through the file, creating the process table along the way, which it returns.
 *
 *
 * @param - *size - size is a pointer to an int which stores the size. firstTrace() changes the int stored at the pointer address
 * @param - filePath is the file path
 * @return - struct Process ** - this is the array of process structs that the method returns based on the trace
 */
struct Process **firstTrace(int *size, char filePath[]){

	int lineNums = 0;
	int currentSize = 0;
	const int rows = 20;
	
	struct Process **traceTable = malloc(sizeof(struct Process *) * rows);
	if(traceTable == NULL){
		fprintf(stderr, "Error: malloc failed.\n");
		exit(1);
	}
	for(int i = 0; i < rows; i++){
		traceTable[i] = createProcess();
	}	
	
	FILE *file;
	file = fopen(filePath, "r");
	if(!file){
		fprintf(stderr, "Error: cannot open file, possibly it does not exist.");
		exit(1);
	}

	bool endOfFileFound = false;
	//bool readingPID = false;
	long int thisPosition = 0;
	bool pidInArray = false;

	char **numArray;
	char *pid;



	while(!endOfFileFound){
		
		char buffer[500];
		thisPosition = 0;
		thisPosition = ftell(file);
		if(fgets(buffer, 500, file) != NULL){
			lineNums++;
			pid = malloc(sizeof(char)*8);

			numArray = parseLineForNums(buffer);

			strcpy(pid, numArray[0]);
			free(numArray[0]);
			free(numArray[1]);
			free(numArray);

			if(currentSize == 0){

				traceTable[0]->PID = pid;
				traceTable[0]->startPosition = thisPosition;
				traceTable[0]->endPosition = thisPosition;
				traceTable[0]->memoryReads = 1;
				currentSize++;
				// update the first and last index
			} else{
				pidInArray = false;
				for(int i = 0; i < currentSize; i++){
					if(strcmp(traceTable[i]->PID, pid) == 0){
						// pid is already in structure
						// update last index
						traceTable[i]->endPosition = thisPosition;
						traceTable[i]->memoryReads++;
						pidInArray = true;
						
						break;
					} 
				}
				if(pidInArray == false){
					
					traceTable[currentSize]->PID = pid;
					traceTable[currentSize]->startPosition = thisPosition;
					traceTable[currentSize]->endPosition = thisPosition;
					traceTable[currentSize]->memoryReads++;
					currentSize++;
				}
			}
			
		} else {
			endOfFileFound = true;
		}
	
	}

	// print, close and make sure it closed correctly
	if(fclose(file) != 0){
		printf("Trace file stream failed to close.\n");
		exit(1);
	}
	*size = currentSize;
	return traceTable;
}



/**
 * mainTrace takes care of the main trace, doing memory reads, putting into the disk queue, taking out of the disk queue, evicting (calling 
 * these functions of course).  
 *
 * @param - theFilePath - trace file
 * @param - int PPNcount - the amount of PPNs for the IPT (size)
 */
void mainTrace(char *theFilePath, int PPNcount){

	struct Statistics *stats = createStatistics();
	

	int *processTableSizePtr = malloc(sizeof(int));
	if(processTableSizePtr == NULL){
		fprintf(stderr, "Error: malloc failed.\n");
	}
	
	// get the process Table from the first trace
	struct Process **processTable = firstTrace(processTableSizePtr, theFilePath);
	int processTableSize = *processTableSizePtr;
	free(processTableSizePtr);

	int processTableUsed = processTableSize;

	FILE *traceFile;
	traceFile = fopen(theFilePath, "r");
	if(!traceFile){
		fprintf(stderr, "Error, cannot open file!");
		exit(EXIT_FAILURE);
	}

	// create all of the structures we need	
	struct DiskQueue *theDiskQueue = createDiskQueue();
	struct InvertedPageTable *IPT = createInvertedPageTable(PPNcount);
	void *algorithmQueue = createAlgorithmQueue(PPNcount);		


	char buffer[50];
	bool endOfFileFound = false;
	int processCompletedCount = 0;

	long int thisPosition = 0;
	thisPosition = fseek(traceFile, 0, SEEK_SET);
	int blockedProcessCount = 0;
	char *pid;
	char *vpn;
	char **numArray;
	bool processInTable;
	int currentProcessIndex;
	
	while(!endOfFileFound && processCompletedCount < processTableSize){
		thisPosition = ftell(traceFile);
		if(fgets(buffer, 50, traceFile) != NULL){
			pid = malloc(sizeof(char) * 15);
			if(pid == NULL){
				fprintf(stderr, "Error: malloc failed.\n");
				exit(1);
			}
			vpn = malloc(sizeof(char) * 15);
			if(vpn == NULL){
				fprintf(stderr, "Error: malloc failed\n");
				exit(1);
			}

			
			
			// copy everything into char * and then free the arrays
			numArray = parseLineForNums(buffer);
			strcpy(pid, numArray[0]);
			strcpy(vpn, numArray[1]);
			free(numArray[0]);
			free(numArray[1]);
			free(numArray);

			char **endPtr = malloc(sizeof(char *)*3);
			unsigned long vpnULong = strtoul(vpn, endPtr, 10);
			free(endPtr);
			//quickly find the spot in the process table (trace table)
			processInTable = false;
			for(int i = 0; i < processTableSize; i++ ){
				if(strcmp(processTable[i]->PID, pid) == 0){
					// this element is the pidpid
					processInTable = true;
					currentProcessIndex = i;
				} 
			}
			if(processInTable == false){
				fprintf(stderr, "Error: current line's PID was not found in the process table at line read.\n");
				exit(EXIT_FAILURE);
			}
			// check to see if thisPosition < startPosition. Because we are constantly reassigning the first position,
			// if we are beyond the start position we have already read 
			if(thisPosition < processTable[currentProcessIndex]->startPosition){
				continue; // this forces us to just move on to the next one without reading
			}

			struct PageTableNode *searchNode; 
			if(processTable[currentProcessIndex]->blocked == true){
				// just continue if its blocked
				continue; // this forces us to just move on to the next one without reading

			} else {
				searchNode = pageTableSearch(processTable[currentProcessIndex]->thePageTable, vpnULong);
				if(searchNode != NULL){
					// reference is in page table, SUCCESSFUL MEMORY READ
					stats->totalMemoryReads++;

					void *nodeBeingReferenced = getIPTAlgoPtr(IPT, searchNode->PPN);
					add(algorithmQueue, searchNode->PPN, 1, nodeBeingReferenced);
					if(thisPosition == processTable[currentProcessIndex]->endPosition){
						// in the case that the current read is the last for this process.
						processCompletedCount++;
						processTableUsed--;
						processTable[currentProcessIndex]->startPosition = thisPosition+1;// this will make it skip over all the lines for this process in the future
						tdestroy(processTable[currentProcessIndex]->thePageTable->rootNode, freePageTableNode);
					} else{
						processTable[currentProcessIndex]->startPosition = thisPosition;
					}
					
					
				} else {
					// WE DID NOT FIND THE PAIR IN THE PAGE TABLE, BLOCK AND ADD THE PROCESS TO DISK QUEUE
					// add the reference to the disk queue
					stats->totalPageFaults++;
					
					insertIntoDiskQueue(theDiskQueue, pid, vpn, thisPosition, currentProcessIndex);
					processTable[currentProcessIndex]->blocked = true;
					blockedProcessCount++;
				}
				 
			}
			
			// increment the clock, and do other stats stuff
			stats->clock = stats->clock +1;
			stats->unblockedTickTotal = stats->unblockedTickTotal + (processTableUsed-blockedProcessCount);
			stats->pagesTickTotal = stats->pagesTickTotal + IPT->currentlyAllocated;

			if(theDiskQueue->diskQueueSize > 0){
				theDiskQueue->countDown = theDiskQueue->countDown - 1;
			}
			
			// this is the check to see if everythign is watiting. If it is, just let the clock go down 
			if(blockedProcessCount == processTableUsed && blockedProcessCount != 0){
					// increment the counter whatever is in the disk queue countdown
					long int leftoverCount = theDiskQueue->countDown;
					theDiskQueue->countDown = 0;
					stats->clock = stats->clock + leftoverCount;
					stats->unblockedTickTotal = stats->unblockedTickTotal + leftoverCount*(processTableUsed-blockedProcessCount);
					stats->pagesTickTotal = stats->pagesTickTotal + leftoverCount*(IPT->currentlyAllocated);


			} 
			
			/**
			 * HERE CHECK IF TIME IS UP ON THE DISK QUEUE. IF IT IS WE GOTTA 
			 * TAKE SOMETHING OUT AND PUT IT IN PHYSICAL MEM. PHYSICAL MEM MAY 
			 * BE FULL SO WE GOTTA CHECK THAT TOO. 
			 * 
			 */

			// if this loop is not taken, the position pointer is just set to the next line, which means it does nothing.
			if(theDiskQueue->countDown == 0 && theDiskQueue->diskQueueSize != 0){
				// the countdown is up so grab something from the disk queue
				struct DiskQueueNode *removedFromDiskNode = removeFromDiskQueue(theDiskQueue);
				long int nextPosition = removedFromDiskNode->filePosition;
				blockedProcessCount--;
				int removedFromDiskIndex = removedFromDiskNode->processTableIndex;
				//quickly find the spot in the process table (trace table)
				
				char **endPtr3 = malloc(sizeof(char *)*3);
				unsigned long vpnULongFromDisk = strtoul(removedFromDiskNode->vpn, endPtr3, 10);
				free(endPtr3);

				char *fromDiskPid = malloc(sizeof(char) * 15);
				strcpy(fromDiskPid, removedFromDiskNode->pid);

				char *fromDiskVpn = malloc(sizeof(char) * 15);
				strcpy(fromDiskVpn, removedFromDiskNode->vpn);
				
				free(removedFromDiskNode->pid);
				free(removedFromDiskNode->vpn);
				free(removedFromDiskNode);
	
				processTable[removedFromDiskIndex]->blocked = false;
				
				if(IPT->size == IPT->currentlyAllocated){
					// the inverted page table is full
					long evictedPPN = evict(algorithmQueue);
					if(evictedPPN == -1){
						printf("PPN is -1\n");
					}
					
					struct InvertedTableNode *evictedNode = getIPTNode(IPT, evictedPPN);
					
					// convert to an unsigned long, thats how its stored/retrieved from the page table
					char **endPtr2 = malloc(sizeof(char *)*3);
					unsigned long vpnULongEvicting = strtoul(evictedNode->vpn, endPtr2, 10);
					free(endPtr2);

					processInTable = false;
					int evictedProcessIndex;
					for(int i = 0; i < processTableSize; i++ ){
						if(strcmp(processTable[i]->PID, evictedNode->pid) == 0){
							// this element is the pid
							evictedProcessIndex = i;
							processInTable = true;
						} 
					}
					if(processInTable == false){
						fprintf(stderr, "Error: current line's PID was not found in the process table while trying to evict.\n");
						exit(EXIT_FAILURE);
					}

					freePPN(IPT, evictedPPN);
					int res = pageTableDelete(processTable[evictedProcessIndex]->thePageTable, vpnULongEvicting);
					if(res != 0){
						printf("unsuccessful deletion.\n");
					}

				}
				
				// now weve checked if the IPT is full, and evicted if we needed to. now add
				long newPPN = getNewPPN(IPT, fromDiskPid, fromDiskVpn);
				void *nodePtr = add(algorithmQueue, newPPN, 0, NULL);
				if(nodePtr == NULL){
					// bad
				}
				//stats->clock = stats->clock + 1;
				setPTAlgoPtr(IPT, newPPN, nodePtr);
				pageTableInsert(processTable[removedFromDiskIndex]->thePageTable, vpnULongFromDisk, newPPN, IPT, algorithmQueue);
				// now weve added the one that was ejected from the disk
				// TO DO: reset the countdown clock
				theDiskQueue->countDown = 2000000;
				if(fseek(traceFile, nextPosition, SEEK_SET) != 0){
					// fseek did not work
					fprintf(stderr, "Error: Could not move the position pointer to the last position for this evicted pid/vpn pair.");
				}
			}
			
		} else {
			endOfFileFound = true; 
		}
	}
	
	// there is a lot of freeing to do, so do that

	free(theDiskQueue);
	freeIPT(IPT);
	free(algorithmQueue);
	// free the process table
	
	for(int i = 0; i < processTableSize; i++){
		if(processTable[i] == NULL){
			continue;
		} else {
			free(processTable[i]->PID);
			free(processTable[i]);
		}
	}
	free(processTable);

	printStatistics(stats);
	free(stats);

	if(fclose(traceFile) != 0){
		fprintf(stderr, "Error: Failed to close the trace file.\n");
		exit(EXIT_FAILURE);
	}


}


/**
 * main handles the input arguments for the command line, setting the pageSize and memorySize vars. Then it calls the trace function
 */
int main(int argc, char *argv[]){
	//char option;
	int pageSize = 4096;
	int memorySize = 1;

	char option;
	
	// start the while loop to get all the options from the command line
	while((option = getopt(argc, argv, "m:p:")) != -1){
		// it is the page size option
		if(option == 'p'){
			// page size
			if(optarg){
				pageSize = atoi(optarg);
				
				if(pageSize == 0){
					fprintf(stderr, "Error: page size not specified. Could not convert to int.\n");
					exit(1);
				}
			} else {
				fprintf(stderr, "Error: page size not specified. Page size after flag not specified.\n");
				exit(1);
			}
			
		} else if(option == 'm'){
			// physical memory
			if(optarg){
				memorySize = atoi(optarg);
				if(memorySize == 0){
					fprintf(stderr, "Error: physical memory size not specified. Could not convert to int.\n");
					exit(1);
				}
			} else {
				fprintf(stderr, "Error: physical memory size not specified. Memory size after flag not specified.\n");
				exit(1);
			}
		}else {
			fprintf(stderr, "Error: flag not m or p. Invalid\n");
			exit(1);
		}
	}
	char *fileName = malloc(sizeof(char) * 20);
	strcpy(fileName, argv[argc-1]);
	int thisPPNCount = (memorySize * 1000000)/ pageSize;
	mainTrace(fileName, thisPPNCount);
	return 0;

}

