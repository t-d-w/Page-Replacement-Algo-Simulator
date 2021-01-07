/**
 * InvertedPageTable.h
 *
 * Author - Thomas Wilkinson
 * CS Login: twilkinson
 * Author - Na Li
 * CS Login: na
 * 
 * Description: This file contains definitions for the inverted page table. In addition to the IPT, there is a free list that contains all of the 
 * free PPNs, and takes out of that list to give one. pid, vpn, and an algorithm node pointer are stored in the IPT
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "InvertedPageTable.h"

/**
 * createPPNFreeList() creates the free list for this inverted page table
 * 
 * @return - the free list for this inverted page table
 */
struct PPNFreeList *createPPNFreeList(){
	struct PPNFreeList *freeList = (struct PPNFreeList *)malloc(sizeof(struct PPNFreeList));
	if(freeList == NULL){
		fprintf(stderr, "Error: malloc failed.\n");
	}
	freeList->head = NULL;
	freeList->tail = NULL;
	freeList->freeListSize = 0;
	return freeList;
}	

/**
 * evictFromFreeList() takes out a free PPN from the free list
 * 
 * @return - long - the PPN chosen to be used
 * @param - currentFreeList- is the current free list that is being used
 */ 
long evictFromFreeList(struct PPNFreeList *currentFreeList){
	if(currentFreeList->freeListSize == 0){
		// its empty so return NULL
		return -1;
	} else if(currentFreeList->freeListSize == 1){
		struct PPNFreeListNode *removedNode = currentFreeList->head;
		currentFreeList->head = NULL;
		currentFreeList->tail = NULL;
		currentFreeList->freeListSize = 0;
		long returnPPN = removedNode->PPN;
		free(removedNode);
		return returnPPN;		
	} else {
		struct PPNFreeListNode *removedNode = currentFreeList->head;
		currentFreeList->head = removedNode->nodeBehind;
		currentFreeList->head->nodeAhead = NULL;
		currentFreeList->freeListSize--;
		long returnPPN = removedNode->PPN;
		free(removedNode);
		return returnPPN;
	}
}

/**
 * addToFreeList() adds to the free list, this happens when a PPN is free of its PPN->PID,VPN
 * 
 * @param currentFreeList is the current free list being used by the IPT
 * @param inputPPN is the PPN you want to add to the free list
 */ 
void addToFreeList(struct PPNFreeList *currentFreeList, long inputPPN){
	struct PPNFreeListNode *newNode = (struct PPNFreeListNode *)malloc(sizeof(struct PPNFreeListNode));
	if(newNode == NULL){
		fprintf(stderr, "malloc failed addToFreeList creating node.\n");
	}
	newNode->PPN = inputPPN;

	// this is just inserting into a doubly linked list
	if(currentFreeList->freeListSize == 0){
		// empty list
		currentFreeList->head = newNode;
		currentFreeList->tail = newNode;
		newNode->nodeAhead = NULL;
		newNode->nodeBehind = NULL;
		currentFreeList->freeListSize = 1;
	} else  {
		newNode->nodeAhead = currentFreeList->tail;
		currentFreeList->tail->nodeBehind = newNode;
		currentFreeList->tail = newNode;
		newNode->nodeBehind = NULL;
		currentFreeList->freeListSize++;
	}
}

/**
 * createInvertedPageTable creates the IPT given the size
 * 
 * @param - inputSize - is the size that the IPT should be
 * @return - struct InvertedPageTable * - the inverted page table you are creating
 */ 
struct InvertedPageTable *createInvertedPageTable(int inputSize){
	// allocate a struct
	struct InvertedPageTable *newIPT = (struct InvertedPageTable *)malloc(sizeof(struct InvertedPageTable));
	if(newIPT == NULL){
		fprintf(stderr, "Error: malloc failed.\n");
		exit(1);
	}

	newIPT->size = inputSize;
	newIPT->currentlyAllocated = 0;
	// build the table
	newIPT->table = (struct InvertedTableNode **)malloc(sizeof(struct InvertedTableNode *) * newIPT->size);
	if(newIPT->table == NULL){
		fprintf(stderr, "Error: malloc failed.\n");
		exit(1);
	}
	// create nodes for the entire table
	for(int i = 0; i < newIPT->size; i++){
		newIPT->table[i] = (struct InvertedTableNode *)malloc(sizeof(struct InvertedTableNode));
		if(newIPT->table[i] == NULL){
			fprintf(stderr, "Error: malloc failed.\n");
			exit(1);
		}
	}
	// create a free list
	// EVERYTHING STARTS OUT FREE, SO FILL THE FREE LIST WITH EMTPY NODES
	newIPT->thisTableFreeList = createPPNFreeList(); 
	for(int i = 0; i < newIPT->size; i++){
		long newPPN = (long)i; // the PPN is the just the index
		newIPT->table[newPPN]->pid = NULL;
		newIPT->table[newPPN]->vpn = NULL;
		newIPT->table[newPPN]->algorithmNode = NULL;
		newIPT->table[newPPN]->reference = 0;
		addToFreeList(newIPT->thisTableFreeList, newPPN);
		
	}
	return newIPT;

}

/**
 * getNewPPN gets a new free ppn from the free list, and sets the entry in the IPT to the new input pid and vpn
 * 
 * @param currentIPT is the current IPT being used
 * @param inputPID is the pid you want to set the entry to have
 * @param inputVpn is the vpn you want to set the entry to have
 * @return long - the ppn where you put the pid vpn pair
 */ 
long getNewPPN(struct InvertedPageTable *currentIPT, char *inputPid, char *inputVpn){
	if(currentIPT->size == 0){
		return -1;
	} else {
		// grab from the free list
		long newPPN = evictFromFreeList(currentIPT->thisTableFreeList);
		if(currentIPT->table[newPPN] == NULL){
			currentIPT->table[newPPN] = (struct InvertedTableNode *)malloc(sizeof(struct InvertedTableNode));
			if(currentIPT->table[newPPN] == NULL){
				fprintf(stderr, "malloc failed.\n");
				exit(EXIT_FAILURE);
			}
		}
		currentIPT->table[newPPN]->pid = inputPid;
		currentIPT->table[newPPN]->vpn = inputVpn;
		currentIPT->table[newPPN]->reference = 1;
		currentIPT->table[newPPN]->algorithmNode = NULL;
		// dont set the algorithm node yet
		currentIPT->currentlyAllocated++;
		return newPPN;
	}
}

/**
 * setPTAlgoPtr() sets the algorithm pointer for an entry in the IPT
 * 
 * @param currentIPT the current ipt we are using
 * @param PPN, the PPN that you want to search for
 * @param void *algoPtr is the algorithm pointer that you are putting in the IPT
 */ 
void setPTAlgoPtr(struct InvertedPageTable *currentIPT, long PPN, void *algoPtr){
	currentIPT->table[PPN]->algorithmNode = algoPtr;
}

/**
 * getPTAlgoPtr() sets the algorithm pointer for an entry in the IPT
 * 
 * @param currentIPT the current ipt we are using
 * @param PPN, the PPN that you want to search for
 * @return void *algoPtr is the algorithm pointer that you are getting from the IPT
 */ 
void *getIPTAlgoPtr(struct InvertedPageTable *current, long PPN){
	return (void *)current->table[PPN]->algorithmNode;
}

/**
 * freePPN() frees the pid and vpns for the current PPN entry. DOES NOT FREE
 * THE ALGO PTR
 * 
 * @param currentIPT the current ipt we are using
 * @param PPN, the PPN that you want to search for
 */ 
void freePPN(struct InvertedPageTable *currentIPT, long PPN){
	currentIPT->table[PPN]->pid = NULL;
	free(currentIPT->table[PPN]->pid);

	currentIPT->table[PPN]->vpn = NULL;
	free(currentIPT->table[PPN]->vpn);
	
	currentIPT->table[PPN]->reference = 0;
	currentIPT->currentlyAllocated--;
	
	addToFreeList(currentIPT->thisTableFreeList, PPN);
}

/**
 * getIPTNode returns the InvertedTableNode in the IPT
 * 
 * @param currentIPT the current ipt we are using
 * @param PPN, the PPN that you want to search for
 */ 
struct InvertedTableNode *getIPTNode(struct InvertedPageTable *currentIPT, long PPN){
	return currentIPT->table[PPN];
}

/**
 * goes through and frees each thing in the table. Then frees the table
 * 
 * @param currentIPT the current ipt we are using
 */ 
void freeIPT(struct InvertedPageTable *currentIPT){
	for(int i = 0; i < currentIPT->size; i++){
		free(currentIPT->table[i]);
	}
	free(currentIPT->table);
	free(currentIPT->thisTableFreeList);
	free(currentIPT);
}


