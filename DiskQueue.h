/**
 * DiskQueue.h
 *
 * Author - Thomas Wilkinson
 * Author - Na Li
 * 
 * Description: This file contains structs and declarations for the disk queue, which stores diskQueueNodes contained pid vpn pairs, and where to jump 
 * back to in the file. It is a doubly linked list and has methods to insert and grab from the list. It is FIFO
 */

#ifndef DISK_QUEUE_H
#define DISK_QUEUE_H

struct DiskQueueNode{
	struct DiskQueueNode *nodeAhead;
	struct DiskQueueNode *nodeBehind;
	long int filePosition; // position to jump back to 
	char *pid;
	char *vpn;
	int processTableIndex; // the index in the process table for fast access
};

struct DiskQueue{
	long countDown; // the time left before something should be ejected from the disk QUueue
	struct DiskQueueNode *head;
	struct DiskQueueNode *tail;
	int diskQueueSize;
};

struct DiskQueue *createDiskQueue();

void insertIntoDiskQueue(struct DiskQueue *currentDiskQueue, char *inputPID, char *inputVPN, long int inputFilePosition, int inputProcessTableIndex);

struct DiskQueueNode *removeFromDiskQueue(struct DiskQueue *currentDiskQueue);

void freeDiskQueue(struct DiskQueue *currentDiskQueue);


#endif




