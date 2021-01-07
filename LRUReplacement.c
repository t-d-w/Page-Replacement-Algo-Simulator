/**
 * LRUReplacement.c
 *
 * Author - Thomas Wilkinson
 * Author - Na Li
 * 
 * Description: The module contains functiions that help to create the LRU queue, add or remove node in the queue.
 */


#include <stdlib.h>
#include <stdio.h>
#include "PageReplacementAlgorithm.h"

/**
 * struct the LRUQueue node  
 */
struct LRUNode{
	struct LRUNode *prev;
	struct LRUNode *next;
	long PPN;
};

/**
 * struct the HashTable
 */
struct HashTable{
        int capacity;
        struct LRUNode **array;
};

/**
 * struct the LRUQueue
 */
struct LRUQueue{
	long count;
	long capacity;
	struct LRUNode *head;
	struct LRUNode *tail;
	struct HashTable *theHashTable;
};

/**
 * create the HashTable
 * @param - maxSize, the capacity of the hashtable
 * @return- void* pointing to the created hashtable
 */
void * createHashTable(int maxSize){
        struct HashTable* hashTable = (struct HashTable *)malloc(sizeof(struct HashTable));
        hashTable->capacity = maxSize;
        hashTable->array = (struct LRUNode **)malloc (sizeof(struct LRUNode) * hashTable->capacity);
        for(int i=0; i<hashTable->capacity; i++){
                hashTable->array[i] = NULL;
        }
        return (void *)hashTable;
}

/**
 * creates the algorithm queue, and returns a void * to the algorithm queue struct
 * @param - maxSize, the capacity of the queue
 * @return- void* pointing to the created queue
 */
void *createAlgorithmQueue(long maxSize){
        struct LRUQueue *lruQueue = (struct LRUQueue*)malloc(sizeof(struct LRUQueue));
        lruQueue->head = NULL;
        lruQueue->tail = NULL;
        lruQueue->count = 0;
        lruQueue->capacity = maxSize;
	lruQueue->theHashTable = createHashTable(maxSize);
	return (void *) lruQueue;
}

/**
 * creates the node in the LRU queue
 * @param - inputPPN, the PPN needs for the new node
 * @return- void* pointing to the created new node
 */
void *createNode(long inputPPN){
	struct LRUNode* newNode = (struct LRUNode *) malloc(sizeof(struct LRUNode));
	if(newNode == NULL){
		fprintf(stderr, "Error: malloc failed.\n");
		exit(1);
	}
	newNode->PPN = inputPPN;
	newNode->prev = NULL;
	newNode->next = NULL;
	return (void *) newNode;
}	

/**
 * chooses which ppn to evict, returns it
 * @param - *currentQueue, pointer to the current LRU queue
 * @return - -1 if LRU queue is empty; otherwise return the PPN of the node evicted
 */
long evict(void *currentQueue){
	struct LRUQueue *CurrentQueue = (struct LRUQueue *)currentQueue;
	// if currentQueue is empty
	if(CurrentQueue->head == NULL){
	//	printf("The lruQueue is empty.\n");
		return -1;
	}

	struct LRUNode *temp;
	
	// if currentQueue is not empty but head == tail
	if(CurrentQueue->head == CurrentQueue->tail){
		temp = CurrentQueue->head;
		CurrentQueue->theHashTable->array[temp->PPN] = NULL;
		CurrentQueue->head = NULL;
		CurrentQueue->tail = NULL;
		CurrentQueue->count--;
		
		return temp->PPN;
	}
	// else
	temp = CurrentQueue->tail;
	CurrentQueue->theHashTable->array[temp->PPN] = NULL;
	CurrentQueue->tail = CurrentQueue->tail->prev; // remove the lruNode at the tail of the Queue;
	CurrentQueue->tail->next = NULL;
	CurrentQueue->count--;
        return temp->PPN;
}

/**
 * adds the PPN to the algorithm
 * @param - *currentQueue, pointer to the current LRU queue
 * @param - inputPPN, the PPN needs for the new node
 * @param - memoryReference, 1 if process in memory, 0 otherwise;
 * @param - *memoryReferenceNode, used for clock algorithm 
 * @return - void * pointing to the newly added LRU node
 */
void *add(void *currentQueue, long inputPPN, int memoryReference, void *memoryReferenceNode){
 	struct LRUQueue *CurrentQueue = (struct LRUQueue *)currentQueue;
 	// check to see if requiredPage is already in the queue. if yes, move the node to the head; if no, add the node the head;
        struct LRUNode *requiredPage = CurrentQueue->theHashTable->array[inputPPN];

	// requiredPage is not in the lruQueue
	if(requiredPage == NULL){
		// if the lruQueue if full, need to delete node first then add the node to the head of the lruQueue;
		if(CurrentQueue->capacity == CurrentQueue->count){
			CurrentQueue->theHashTable->array[CurrentQueue->tail->PPN] = NULL;
			//printf("before delete a node\n");
			evict(CurrentQueue);// when need to delete the LRU node, delelte the tail;
		}

		struct LRUNode *temp = createNode(inputPPN);

		temp->next = CurrentQueue->head;
		// if the lruQueue is empty
		if(CurrentQueue->count == 0){
                	CurrentQueue->head = temp;
              		CurrentQueue->tail = temp;
			//return (void *) CurrentQueue->head;
                //	CurrentQueue->tail->next = NULL;
		} else { // lruQueue is not empty, add to head
			CurrentQueue->head->prev = temp;
			CurrentQueue->head = temp;
			//return (void *) CurrentQueue->head;
		}
		CurrentQueue->theHashTable->array[inputPPN] = temp;
		CurrentQueue->count++;
	} else{// requirePage is in the lruQueue
		if(requiredPage == CurrentQueue->head){
			//return NULL;
		}else{
			// if the requiredPage is not the head of the Queue, need to remove it from the original place to the head of the queue
			// remove
			requiredPage->prev->next = requiredPage->next;
			if(requiredPage->next != NULL){// requiredPage is not the tail
				requiredPage->next->prev = requiredPage->prev;
			} else{
				CurrentQueue->tail = requiredPage->prev;
				CurrentQueue->tail->next = NULL;
			}

			// add to the head of the queue
			requiredPage->next = CurrentQueue->head;
			requiredPage->prev = NULL;
			CurrentQueue->head->prev = requiredPage;
			CurrentQueue->head = requiredPage;
		}
	}
	return (void *)CurrentQueue->head;

}

/**
 * remove a specific node in the LRUQueue (not just the tail)
 * @param - *currentQueue, pointer to the current LRU queue
 * @param - *specificNode, the specific node to be removed
 * @return - void * pointing to the newly added LRU node
 */
int evictSpecific(void *currentQueue, void *specificNode){
	struct LRUQueue *properCurrentQueue = (struct LRUQueue *)currentQueue;
        struct LRUNode *properCurrentNode = (struct LRUNode *)specificNode;
	if(properCurrentQueue->count == 0){
                return 0;
        } else if(properCurrentQueue->count == 1 || properCurrentNode->PPN == properCurrentQueue->head->PPN){
                evict((void *)properCurrentQueue);
                return 1;
        } else if(properCurrentQueue->tail->PPN == properCurrentNode->PPN){
                properCurrentQueue->tail = properCurrentNode->prev;
		properCurrentNode->prev->next = NULL;
		properCurrentQueue->theHashTable->array[properCurrentNode->PPN] = NULL;
                properCurrentQueue->count--;
                return 1;
        } else {
		if(properCurrentQueue->theHashTable->array[properCurrentNode->PPN] != NULL){
			struct LRUNode *temp = properCurrentQueue->theHashTable->array[properCurrentNode->PPN];
			if(temp->next == NULL){
			//	printf("temp ppn: %li, temp->next ppn: %li, temp->next->prev ppn: %li\n", temp->PPN, temp->next->PPN, temp->next->prev->PPN);
			}
			temp->next->prev = temp->prev;
			
			temp->prev->next = temp->next;
                	properCurrentQueue->count--;
			properCurrentQueue->theHashTable->array[properCurrentNode->PPN] =NULL;
		
			return 1;
		} else {
			return 0;
		}
                
        }

}



