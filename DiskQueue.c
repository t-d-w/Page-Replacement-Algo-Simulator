/**
 * DiskQueue.c
 *
 * Author - Thomas Wilkinson
 * CS Login: twilkinson
 * Author - Na Li
 * CS Login: na
 * 
 * Description: This file contains definitions for the disk queue, which stores diskQueueNodes contained pid vpn pairs, and where to jump 
 * back to in the file. It is a doubly linked list and has methods to insert and grab from the list.
 */

#include <stdlib.h>
#include <stdio.h>
#include "DiskQueue.h"

/**
 * createDIskQueue creates the diskQueue
 * 
 * @return struct DiskQueue * - the Disk Queue the main file will be using
 */ 
struct DiskQueue *createDiskQueue(){
	struct DiskQueue *newDiskQueue = (struct DiskQueue *)malloc(sizeof(struct DiskQueue));
	if(newDiskQueue == NULL){
		fprintf(stderr, "Error: malloc failed.\n");
		exit(1);
	}
	newDiskQueue->countDown = 2000000;
	newDiskQueue->head = NULL;	
	newDiskQueue->tail = NULL;
	newDiskQueue->diskQueueSize = 0;
	return newDiskQueue;
}

/**
 * insertIntoDiskQueue() inserts the pid,vpn pair into the disk queue.
 * 
 * @param - currentDiskQueue is the queue we are using
 * @param - inputPID - the pid to insert
 * @param - inputVPN - the vpn to insert
 * @param - inputFilePosition - the position that we should jump back to when we take it out of the queue
 * @param - inputProcessTableIndex - we store the index in the process table that the pid is, making it fast when we take out of the queue
 */ 
void insertIntoDiskQueue(struct DiskQueue *currentDiskQueue, char *inputPID, char *inputVPN, long int inputFilePosition, int inputProcessTableIndex){
	struct DiskQueueNode *newDiskQueueNode = (struct DiskQueueNode *)malloc(sizeof(struct DiskQueueNode));
	if(newDiskQueueNode == NULL){
		fprintf(stderr, "Error: malloc failed\n");
		exit(1);
	}
	
	newDiskQueueNode->processTableIndex = inputProcessTableIndex;
	newDiskQueueNode->pid = inputPID;
	newDiskQueueNode->vpn = inputVPN;
	newDiskQueueNode->filePosition = inputFilePosition;
	if(currentDiskQueue->diskQueueSize == 0){
		// disk queue is empty, set
		newDiskQueueNode->nodeAhead = NULL;
		newDiskQueueNode->nodeBehind = NULL;
	    currentDiskQueue->head = newDiskQueueNode;
		currentDiskQueue->tail = newDiskQueueNode;
		currentDiskQueue->diskQueueSize = 1;
	} else {
		newDiskQueueNode->nodeAhead = currentDiskQueue->tail;
		currentDiskQueue->tail->nodeBehind = newDiskQueueNode;
		currentDiskQueue->tail = newDiskQueueNode;
		newDiskQueueNode->nodeBehind = NULL;
		currentDiskQueue->diskQueueSize++;
	}
}

/**
 * removeFromDiskQueue takes out of the disk queue (FIFO) and returns it
 * 
 * @param - currentDiskQueue is the queue we are using
 * @return struct DiskQueueNode * - the node that we are removing
 */ 
struct DiskQueueNode *removeFromDiskQueue(struct DiskQueue *currentDiskQueue){
	if(currentDiskQueue->diskQueueSize == 0){
		// the queue is already empty
		return NULL;
	} else if(currentDiskQueue->diskQueueSize == 1){
		// set the head and tail to null
		struct DiskQueueNode *removedNode = currentDiskQueue->head;
		currentDiskQueue->head = NULL;
		currentDiskQueue->tail = NULL;
		currentDiskQueue->diskQueueSize = 0;
		return removedNode;
	} else {
		struct DiskQueueNode *removedNode = currentDiskQueue->head;
		currentDiskQueue->head = removedNode->nodeBehind;
		currentDiskQueue->head->nodeAhead = NULL;
		currentDiskQueue->diskQueueSize--;
		return removedNode;
	}
}

