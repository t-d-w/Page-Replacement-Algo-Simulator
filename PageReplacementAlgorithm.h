/**
 * PageReplacementAlgorithm.h
 *
 * Author - Thomas Wilkinson
 * CS Login: twilkinson
 * Author - Na Li
 * CS Login: na
 * 
 * Description: create the structure and methods used for FifoRepalcement.c, LRURepalcement.c, ClockReplacement.c
 *
 *
 */

#ifndef PAGE_REPLACEMENT_ALGORITHM_H
#define PAGE_REPLACEMENT_ALGORITHM_H
#include <stdlib.h>
#include <stdio.h>




/**
 * creates the algorithm queue, and returns a void * to the algorithm queue struct
 * @param - maxSize, the capacity of the queue
 * @return- void* pointing to the created queue
 */
void *createAlgorithmQueue(long maxSize);


/**
 * adds the PPN to the algorithm
 *@param - *currentQueue, pointer to the current queue
 * @param - inputPPN, the PPN needs for the new node
 * @param - memoryReference, 1 if process in memory, 0 otherwise;
 * @param - *memoryReferenceNode, used for the algorithm 
 * @return - void * pointing to the newly added queue
 */
void *add(void *currentQueue, long PPN, int memoryReference, void *memoryReferenceNode);

//void *add(void *currentQueue, long inputPPN, void *currentHash);// this is modified for LRU

/**
 * remove a node(head or tail) in the Queue
 * chooses which ppn to evict, returns it
 * @param - *currentQueue, pointer to the current queue
 * @return - -1 if queue is empty; otherwise return the PPN of the node evicted
 */
long evict(void *currentQueue);

/**
 * remove a specific node in the Queue
 * @param - *currentQueue, pointer to the current queue
 * @param - *specificNode, the specific node to be removed
 * @return - void * pointing to the newly added queue
 */
int evictSpecific(void *currentQueue, void *specificNode);


#endif









