/**
 * FifoReplacement.c
 * Author - Thomas Wilkinson
 * CS Login: twilkinson
 * Author - Na Li
 * CS Login: na
 *	
 * Description: The module contains functiions that help to create the FIFO queue, add or remove node in the queue.
 */

#include <stdlib.h>
#include <stdio.h>
#include "PageReplacementAlgorithm.h"


/**
 * struct the FifoQueue node  
 */
struct FifoQueueNode{
        struct FifoQueueNode *next;
        struct FifoQueueNode *prev;
        long PPN;
};

/**
 * struct the FifoQueue
 */

struct FifoQueue{
        //int countDown;
	long capacity;
        struct FifoQueueNode *head;
        struct FifoQueueNode *tail;
        long FifoQueueSize;
};

/**
 * creates the algorithm queue, and returns a void * to the algorithm queue struct
 * @return - void* pointing to the newly created FifoQueue
 */
void *createAlgorithmQueue(long maxSize){
	struct FifoQueue *newFifoQueue = (struct FifoQueue*)malloc(sizeof(struct FifoQueue));
	newFifoQueue->head = NULL;
	newFifoQueue->tail = NULL;
	newFifoQueue->FifoQueueSize = 0;
	newFifoQueue->capacity = maxSize;
	return (void *)newFifoQueue;
}


/**
 * adds the PPN to the algorithm
 * @param - *currentQueue, pointer to the current Fifo queue
 * @param - inputPPN, the PPN needs for the new node
 * @param - memoryReference, 1 if process in memory, 0 otherwise;
 * @param - *memoryReferenceNode, used for clock algorithm 
 * @return - void * pointing to the newly added Fifonode
 */
void *add(void *currentQueue, long PPN, int memoryReference, void *memoryReferenceNode){
	// if PPN is already in memory
	if(memoryReference == 1){
		return NULL;
	}
	// PPN not in memory
	struct FifoQueue *properCurrentQueue = (struct FifoQueue *)currentQueue;
	if(properCurrentQueue == NULL){
		fprintf(stderr, "the FifoQueue created in add function in FifoAlogorithm is null.\n");
		exit(1);
	}
	struct FifoQueueNode *newFifoNode = (struct FifoQueueNode*)malloc(sizeof(struct FifoQueueNode));
	newFifoNode->PPN = PPN;
	// // check to see if PPN is already in the queue(if PPN exist in IPT, then PPN is in the Queue). if yes, do nothing; if no, add the node the tail; 
        // when to delete the Fifo node, delelte the head;
        
	if(properCurrentQueue->FifoQueueSize == 0){
		// queue is empty
		properCurrentQueue->head = newFifoNode;
		properCurrentQueue->tail = newFifoNode;
		newFifoNode->next = NULL;
		newFifoNode->prev = NULL;
		properCurrentQueue->FifoQueueSize++;
		return (void *)newFifoNode;
	}else{
		newFifoNode->prev = properCurrentQueue->tail;
		newFifoNode->next = NULL;
		properCurrentQueue->tail->next = newFifoNode;
		properCurrentQueue->tail = newFifoNode;
		properCurrentQueue->FifoQueueSize++;
		return (void *)newFifoNode;

	}
}

/**
 * chooses which ppn to evict, returns it
 * @return - ppn of the removed node in the FifoQueue
 *
 */
long evict(void *currentQueue){
	struct FifoQueue *structCurrentQueue = (struct FifoQueue *)currentQueue;
	if(structCurrentQueue == NULL){
                fprintf(stderr, "the FifoQueue created in evict function in FifoAlogorithm is null.\n");
        	exit(1);
	}
	// queue is empty
	if(structCurrentQueue->FifoQueueSize == 0){
		return -1;
	}else if(structCurrentQueue->FifoQueueSize == 1){
		struct FifoQueueNode *removedNode = structCurrentQueue->head;
		if(removedNode == NULL){
                	fprintf(stderr, "the removedNode created in evict function in FifoAlogorithm is null.\n");
        		exit(1);
		}
		long ppn = removedNode->PPN;
		structCurrentQueue->head = NULL;
		structCurrentQueue->tail = NULL;
		structCurrentQueue->FifoQueueSize = 0;
		free(removedNode);
		return ppn;
	}else{
		struct FifoQueueNode *removedNode = structCurrentQueue->head;
		if(removedNode == NULL){
                        fprintf(stderr, "the removedNode created in evict function in FifoAlogorithm is null.\n");
                	exit(1);
		}
        	long ppn = removedNode->PPN;
        	structCurrentQueue->head = structCurrentQueue->head->next;
		structCurrentQueue->head->prev = NULL;
		structCurrentQueue->FifoQueueSize--;
		free(removedNode);
        return ppn;
	}
}

/**
 * remove a specific node in the FifoQue (not just the head)
 * @param - *currentQueue, pointer to the current Fifoqueue
 * @param - *specificNode, the specific node to be removed
 * @return - 1 if successfully removedd; 0 otherwise.
 *
 */
int evictSpecific(void *currentQueue, void *specificNode){
	struct FifoQueue *properCurrentQueue = (struct FifoQueue *)currentQueue;
	if(properCurrentQueue == NULL){
                fprintf(stderr, "the FifoQueue created in evictSpecic function in FifoAlogorithm is null.\n");
        	exit(1);
	}
	struct FifoQueueNode *properCurrentNode = (struct FifoQueueNode *)specificNode;
	if(properCurrentNode == NULL){
                fprintf(stderr, "the FifoNode created in evictSpecific function in FifoAlogorithm is null.\n");
        	exit(1);
	}
	if(properCurrentQueue->FifoQueueSize == 0){
		return 0;
	} else if(properCurrentQueue->FifoQueueSize == 1 || properCurrentNode->PPN == properCurrentQueue->head->PPN){
		evict((void *)properCurrentQueue);
		return 1;
	} else if(properCurrentQueue->tail->PPN == properCurrentNode->PPN){
		properCurrentQueue->tail = properCurrentNode->prev;
		properCurrentNode->prev->next = NULL;
		properCurrentQueue->FifoQueueSize--;
		free(properCurrentNode);
		return 1;
	} else {
		properCurrentNode->next->prev = properCurrentNode->prev;
		properCurrentNode->prev->next = properCurrentNode->next;
		properCurrentQueue->FifoQueueSize--;
		free(properCurrentNode);
		return 1;
	}

}



