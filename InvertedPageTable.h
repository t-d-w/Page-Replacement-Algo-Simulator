/**
 * InvertedPageTable.h
 *
 * Author - Thomas Wilkinson
 * CS Login: twilkinson
 * Author - Na Li
 * CS Login: na
 * 
 * Description: This file contains structs and declarations for the inverted page table. In addition to the IPT, there is a free list that contains all of the 
 * free PPNs, and takes out of that list to give one. pid, vpn, and an algorithm node pointer are stored in the IPT
 */


#ifndef INVERTEDPAGETABLE_H
#define INVERTEDPAGETABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>


struct PPNFreeListNode{
	struct PPNFreeListNode *nodeAhead;
	struct PPNFreeListNode *nodeBehind;
	long PPN;
};

struct PPNFreeList{
	struct PPNFreeListNode *head;
	struct PPNFreeListNode *tail;
	int freeListSize;
};

struct InvertedTableNode{
	char *pid;
	char *vpn;
	int reference;
	void *algorithmNode;
};

struct InvertedPageTable{
	struct InvertedTableNode **table;
	struct PPNFreeList *thisTableFreeList;
	int size;
	int currentlyAllocated;
};

struct InvertedPageTable *createInvertedPageTable(int inputSize);

long getNewPPN(struct InvertedPageTable *currentIPT, char *inputPid, char *inputVpn);

void setPTAlgoPtr(struct InvertedPageTable *currentIPT, long PPN, void *algoPtr);

void *getIPTAlgoPtr(struct InvertedPageTable *current, long PPN);

void freePPN(struct InvertedPageTable *currentIPT, long PPN);

struct InvertedTableNode *getIPTNode(struct InvertedPageTable *currentIPT, long PPN);

void freeIPT(struct InvertedPageTable *currentIPT);

#endif










