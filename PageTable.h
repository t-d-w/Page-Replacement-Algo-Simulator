/**
 * PageTable.h
 *
 * Author - Thomas Wilkinson
 * CS Login: twilkinson
 * Author - Na Li
 * CS Login: na
 * 
 * Description: Contains methods that create the page table, the page table nodes, compare (for tree), free nodes(for tdestroy), search, and delete. 
 *  header file defines the structs PageTableNode and PageTable
 */

#ifndef PAGETABLE_H
#define PAGETABLE_H
#define _GNU_SOURCE

#include <search.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "PageReplacementAlgorithm.h"
#include "InvertedPageTable.h"

/**
 * struct PageTableNode contains the vpn, ppn, the IPT used, and the algo queue used 
 */
struct PageTableNode{
	unsigned long vpn;
	long PPN;
	struct InvertedPageTable *theIPT;
	void *currentAlgoQueue;
};

/**
 * struct pageTable contains the root and current size of the tree 
 */
struct PageTable {
	void *rootNode; // hashtable data that is required for hsearch functions
	int currentSize; // capacity of the hashtable
};

/**
 * compare() compares the two nodes, and returns an int indicating which is bigger, or if they are equal. This will depend 
 * on their vpns
 * 
 * @param - pageTableNode1 - first node
 * @param - pageTableNode2 - second node
 * @return - int - if first node is less than second return negative, vice versa return positive, and return 0 if they are the same
 */
int compare(const void *pageTableNode1, const void *pageTableNode2);

/**
 * createPageTable() creates the page table and returns it
 * 
 * @return - the page table for the process
 */
struct PageTable *createPageTable();

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
int pageTableInsert(struct PageTable *currentTable, unsigned long inputVpn, long inputPpn, struct InvertedPageTable *inputIPT, void *inputAlgoQueue);

/**
 * pageTableSearch
 *
 * search the page table for a key returns null if not found
 * 
 * @param currentTable being used
 * @param inputVPN the vpn youre looking for
 */
struct PageTableNode *pageTableSearch(struct PageTable *currentTable, unsigned long inputVpn);

/**
 * pageTableDelete
 *
 * searches for the page in the tree 
 *
 * @param currentTable the current page table
 * @param inputVpn the vpn you are trying to delete
 * @return - int - 0 upon success
 */
int pageTableDelete(struct PageTable *currentTable, unsigned long inputVpn);

/**
 * freePageTableNode frees the page table node, and also evicts from the algo queue, and frees from the IPT. This is done while tdestroy() is 
 * destroying the tree  
 * 
 * @param - inputNode - the current node that the tree destroyer is on
 */
void freePageTableNode(void *inputNode);

#endif
