/**
 * PageTable.c
 *
 * Author - Thomas Wilkinson
 * Author - Na Li
 * 
 * Description: Contains methods that create the page table, the page table nodes, compare (for tree), free nodes(for tdestroy), search, and delete. 
 *  
 */

#define _GNU_SOURCE
#include "PageTable.h"
#include <stdio.h>
#include <string.h>
#include <search.h>
#include <stdlib.h>

/**
 * compare() compares the two nodes, and returns an int indicating which is bigger, or if they are equal. This will depend 
 * on their vpns
 * 
 * @param - pageTableNode1 - first node
 * @param - pageTableNode2 - second node
 * @return - int - if first node is less than second return negative, vice versa return positive, and return 0 if they are the same
 */
int compare(const void *pageTableNode1, const void *pageTableNode2){
	struct PageTableNode *firstNode = (struct PageTableNode *)pageTableNode1;
	struct PageTableNode *secondNode = (struct PageTableNode *)pageTableNode2;
	
	if(firstNode->vpn < secondNode->vpn){
		return -1;
	} else if(firstNode->vpn > secondNode->vpn){
		return 1;
	} else {
		return 0;
	}

}

/**
 * createPageTable() creates the page table and returns it
 * 
 * @return - the page table for the process
 */
struct PageTable *createPageTable(){
	struct PageTable *pageTable = (struct PageTable *) malloc(sizeof(struct PageTable));
	if(pageTable == NULL){
		fprintf(stderr,"pageTable does not exist.\n");
		exit(1);
	}
	pageTable->currentSize = 0; //capacity is fixed
	pageTable->rootNode = NULL;
	return pageTable;
}

/**
 * pageTableInsert
 *
 * called after returning from disk, we create a new node and insert it into the tree using tsearch. the IPT and ALGO queue are stored
 * in the node to make the freeing process a little bit easier.
 *
 * @return - int - returns 1 if full, returns 0 if succesfully added
 * @param - currentTable - the current page table
 * @param - inputVPN - the input vpn to insert
 * @param - inputPpn - the PPN to insert
 * @param - inputIPT, the IPT being used
 * @param - inputAlgoQueue - the algo queue being used
 */
int pageTableInsert(struct PageTable *currentTable, unsigned long inputVpn, long inputPpn, struct InvertedPageTable *inputIPT, void *inputAlgoQueue){
	// concatenate pid and vpn, add ppn as the data
	// use hsearch
	struct PageTableNode *newNode = (struct PageTableNode *)malloc(sizeof(struct PageTableNode));
	newNode->vpn = inputVpn;
	newNode->PPN = inputPpn;
	newNode->theIPT = inputIPT;
	newNode->currentAlgoQueue = inputAlgoQueue;
	//printf("the return val wasnt null!\n");
	void *returnVal = tsearch(newNode, &currentTable->rootNode, compare);
	if(returnVal != NULL){
		struct PageTableNode *returnValNode = *(struct PageTableNode **)returnVal;
		if(returnValNode != newNode){
			return 1;
		} else {
			//added the node so return 0 and up the size
			currentTable->currentSize++;
			return 0;
		}
	} else {
		fprintf(stderr, "Error!\n");
		exit(EXIT_FAILURE);
	}
	
}

/**
 * pageTableSearch
 *
 * search the page table for a key returns null if not found
 * 
 * @param currentTable being used
 * @param inputVPN the vpn youre looking for
 */
struct PageTableNode *pageTableSearch(struct PageTable *currentTable, unsigned long inputVpn){
	// also use hsearch, specify a different action in the arguments
	
	struct PageTableNode searchWithNode;
	searchWithNode.vpn = inputVpn;
	void *returnVal = tfind(&searchWithNode, &currentTable->rootNode, compare);
	if(returnVal != NULL){
		struct PageTableNode *returnValNode = *(struct PageTableNode **)returnVal;
		return returnValNode;
	} else {
		return NULL;
	}
}

/**
 * pageTableDelete
 *
 * searches for the page in the tree 
 *
 * @param currentTable the current page table
 * @param inputVpn the vpn you are trying to delete
 * @return - int - 0 upon success
 */
int pageTableDelete(struct PageTable *currentTable, unsigned long inputVpn){
	// do we want to store the deleted ppn to keep records of which ppn are available?
	struct PageTableNode *returnValNode = pageTableSearch(currentTable, inputVpn);
	if(returnValNode == NULL){
		return 1;
	} else {
		tdelete(returnValNode, &currentTable->rootNode, compare);
		free(returnValNode);
		currentTable->currentSize--;
		return 0;
	}
}

/**
 * freePageTableNode frees the page table node, and also evicts from the algo queue, and frees from the IPT. This is done while tdestroy() is 
 * destroying the tree  
 * 
 * @param - inputNode - the current node that the tree destroyer is on
 */
void freePageTableNode(void *inputNode){
	struct PageTableNode *castedNode = (struct PageTableNode *)inputNode;
	void *algoPtrToRemove  = getIPTAlgoPtr(castedNode->theIPT, castedNode->PPN);
	evictSpecific(castedNode->currentAlgoQueue, algoPtrToRemove);
	freePPN(castedNode->theIPT, castedNode->PPN);
	free(castedNode);
}

