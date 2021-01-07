/**
 * ClockReplacement.c
 * 
 * Author - Thomas Wilkinson
 * CS Login: twilkinson
 * Author - Na Li
 * CS Login: na
 *      
 * Description: The module contains functiions that help to create the Clock queue, add or remove node in the queue.
 */


#include <stdlib.h>
#include <stdio.h>
#include "PageReplacementAlgorithm.h"

/**
 * struct the ClockQueue node  
 */
struct ClockNode{
	struct ClockNode *prev;
	struct ClockNode *next;
	int refBit;
	long PPN;
};

/**
 * struct the ClockQueue  
 */
struct ClockQueue{
	long count;
	long capacity;
	struct ClockNode *currentPosition;
	struct ClockNode *head;
	struct ClockNode *tail;
};

/**
 * creates the algorithm queue, and returns a void * to the algorithm queue struct
 * @param - maxSize, the capacity of the queue
 * @return- void* pointing to the created queue
 */
void *createAlgorithmQueue(long maxSize){
        struct ClockQueue *clockQueue = (struct ClockQueue *)malloc(sizeof(struct ClockQueue));
        clockQueue->head = NULL;
        clockQueue->tail = NULL;
        clockQueue->count = 0;
        clockQueue->capacity = maxSize;
	return (void *) clockQueue;
}

/**
 * creates the node in the Clock queue
 * @param - inputPPN, the PPN needs for the new node
 * @return- the created new node
 */
struct ClockNode *createNode(long inputPPN){
	struct ClockNode* newNode = (struct ClockNode *) malloc(sizeof(struct ClockNode));
	newNode->PPN = inputPPN;
	newNode->refBit = 1;
	newNode->prev = NULL;
	newNode->next = NULL;
	return newNode;
}	

/**
 * chooses which ppn to evict, returns it
 * @param - *currentQueue, pointer to the current Clock queue
 * @return - -1 if Clock queue is empty; otherwise return the PPN of the node evicted
 *
 */
long evict(void *currentQueue){
	struct ClockQueue *theClockQueue = (struct ClockQueue *)currentQueue;
	// if currentQueue is empty
	struct ClockNode *currentNode = theClockQueue->currentPosition;
	long removePPN;
	
	if(currentNode == NULL){
		return -1;
	}

	while(currentNode->refBit == 1){
		currentNode->refBit = 0;
		currentNode = currentNode->next;
	}
	// the reference bit is 0 so clear it
	struct ClockNode *tempRemove = currentNode;
	if(theClockQueue->count == 0 || tempRemove == NULL){
		return -1;
	} else if(theClockQueue->count == 1){
		
		removePPN = tempRemove->PPN;
		currentNode = NULL;	
		free(currentNode);
		theClockQueue->count--;
		return removePPN;
	} else if(tempRemove->PPN == theClockQueue->head->PPN){
		theClockQueue->head = tempRemove->prev;
		theClockQueue->head->next = theClockQueue->tail;
		theClockQueue->tail->prev = theClockQueue->head;
	} else if(tempRemove->PPN == theClockQueue->tail->PPN){
		theClockQueue->tail = tempRemove->next;
		theClockQueue->tail->prev = theClockQueue->head;
		theClockQueue->head->next = theClockQueue->tail;
	} else {
		if(tempRemove->prev == NULL){
			printf("tempRemove->prev is null, size is %li\n", theClockQueue->count);
		}
		tempRemove->next->prev = tempRemove->prev;
		tempRemove->prev->next = tempRemove->next;
	}
	theClockQueue->count--;
	theClockQueue->currentPosition = tempRemove->next;
	removePPN = tempRemove->PPN;
	free(tempRemove);
	return removePPN;
}

/**
 * adds the PPN to the algorithm
 * @param - *currentQueue, pointer to the current Clock queue
 * @param - inputPPN, the PPN needs for the new node
 * @param - memoryReference, 1 if process in memory, 0 otherwise;
 * @param - *memoryReferenceNode, used for clock algorithm 
 * @return - void * pointing to the newly added ClockNode
 */
void *add(void *currentQueue, long inputPPN, int memoryReference, void *memoryReferenceNode){
	
	struct ClockQueue *theClockQueue = (struct ClockQueue *)currentQueue;

	if(memoryReference == 1){
		struct ClockNode *memoryRefNode = (struct ClockNode *)memoryReferenceNode;
		if(memoryRefNode == NULL){
			
		}
		memoryRefNode->refBit = 1;
		return NULL;
	} else {
		// add right before the current position
		struct ClockNode *newNode = createNode(inputPPN);

		if(theClockQueue->currentPosition == NULL){
			if(theClockQueue->count != 0){
				fprintf(stderr, "something is wrong here. size isnt zero.\n");
				exit(EXIT_FAILURE);
			} 

			theClockQueue->head = newNode;
			theClockQueue->tail = newNode;
			newNode->next = newNode;
			newNode->prev = newNode;
			theClockQueue->currentPosition = newNode;// update refBit?
			theClockQueue->count = 1;
			return (void *)newNode;
		} 
		struct ClockNode *currentNode = theClockQueue->currentPosition;
		
		if(theClockQueue->count == 1){
			// if theres only one just create it going around to itself
			newNode->next = currentNode;
			newNode->prev = currentNode;
			currentNode->prev = newNode;
			currentNode->next = newNode;
			theClockQueue->tail = newNode;

		}else if(theClockQueue->currentPosition->PPN == theClockQueue->head->PPN){
			// if the current position is at th head, just put the newNode right behind it.
			newNode->next = currentNode;
			newNode->prev = currentNode->prev;
			currentNode->prev->next = newNode;
			currentNode->prev = newNode;

		} else if(currentNode->PPN == theClockQueue->tail->PPN){
			// if the current position is at the tail, then put the new node at the head
			newNode->next = currentNode;
			newNode->prev = theClockQueue->head;
			theClockQueue->head->next = newNode;
			theClockQueue->head = newNode;
		} else {
			if(currentNode->prev == NULL){
				printf("currentNode->prev is null, sizE is %li\n", theClockQueue->count);
			} 
			newNode->next = currentNode;
			newNode->prev = currentNode->prev;
			currentNode->prev->next = newNode;
			currentNode->prev = newNode;
		}
		theClockQueue->count++;
		return (void *)newNode;
	}
}

/**
 * remove a specific node in the ClockQueue (not just the tail)
 * @param - *currentQueue, pointer to the current Clock queue
 * @param - *specificNode, the specific node to be removed
 * @return - void * pointing to the newly added Clock node
 */
int evictSpecific(void *currentQueue, void *specificNode){

	struct ClockQueue *theClockQueue = (struct ClockQueue *)currentQueue;
	struct ClockNode *removeNode = (struct ClockNode *)specificNode;
		
	if(theClockQueue->count == 0){
		return  0;
	} else if(theClockQueue->count == 1){
		if(evict(theClockQueue) != -1){
			return 1;
		}
		return 0;
	} else if(theClockQueue->currentPosition->PPN == removeNode->PPN){
		theClockQueue->currentPosition = theClockQueue->currentPosition->next;
		removeNode->prev->next = theClockQueue->currentPosition;
		theClockQueue->currentPosition->prev = removeNode->prev;
		theClockQueue->count--;
		free(removeNode);
		return 1;
	} else {
		printf(" ");	
		removeNode->prev->next = removeNode->next;
		removeNode->next->prev = removeNode->prev;
		theClockQueue->count--;
		free(removeNode);
		return 1;
	}

}


