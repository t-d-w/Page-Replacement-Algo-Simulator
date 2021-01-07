/**
 * Statistics.c
 *
 * Author - Thomas Wilkinson
 * Author - Na Li
 * 
 * Description:The module contains functiions that help toThe module contains functiions that help to calculate the statistics of the project, including AMU, ARP, toal page fault, total memoey reads, total clocl ticks.
 *
 */
#include "Statistics.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * create the Statistics  
 */
struct Statistics *createStatistics(){
	//printf("creating statistics struct\n");
    	struct Statistics *newStats = (struct Statistics *)malloc(sizeof(struct Statistics));
    	newStats->totalMemoryReads = 0;
    	newStats->totalPageFaults = 0;
    	newStats->clock = 0;
	//printf("returning from creating statistics struct\n")
	//;
	
	newStats->unblockedTickTotal = 0;
	newStats->pagesTickTotal = 0;

    	return newStats;
}

/**
 * calculate the ARP
 * @param - *currentStats, point to current Statistics
 * @return - calulated ARP
 */
long double getARP(struct Statistics *currentStats){
	return (currentStats->unblockedTickTotal)/(currentStats->clock);		
}

/**
 * calculate the AMU
 * @param - *currentStats, point to current Statistics
 * @return - calulated AMU, only shows the fraction part (0-1)
 */
long double getAMU(struct Statistics *currentStats){
	return (currentStats->pagesTickTotal)/(currentStats->clock) - (long)((currentStats->pagesTickTotal)/(currentStats->clock));
}

/**
 * print the statistics
 * @param - *currentStats, point to current Statistics
 * 
 */
void printStatistics(struct Statistics *currentStats){
    	printf("\nClock ticks: %li\nTotal Memory Reads: %i\nTotal Page Faults: %i\nAMU: %Lf\nARP: %Lf\n", currentStats->clock, currentStats->totalMemoryReads, currentStats->totalPageFaults, getAMU(currentStats), getARP(currentStats));
}



