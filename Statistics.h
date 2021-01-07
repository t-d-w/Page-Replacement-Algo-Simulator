/**
 * Statistics.h
 *
 * Author - Thomas Wilkinson
 * CS Login: twilkinson
 * Author - Na Li
 * CS Login: na
 * 
 * Description:The module contains functiions that help toThe module contains functiions that help to calculate the statistics of the project, including AMU, ARP, toal page fault, total memoey reads, total clocl ticks.
 */
#ifndef STATISTICS_H
#define STATISTICS_H

/**
 * struct the Statistics
 */
struct Statistics{
    	int totalMemoryReads;
    	int totalPageFaults;
    	long clock;
	long double unblockedTickTotal; // for the ARP
	long double pagesTickTotal; // for the AMU
	
};

/**
 * create the Statistics
 */
struct Statistics *createStatistics();

/**
 * calculate the ARP
 * @param - *currentStats, point to current Statistics
 * @return - calulated ARP
 */
long double getARP(struct Statistics *currentStats);

/**
 * calculate the AMU
 * @param - *currentStats, point to current Statistics
 * @return - calulated AMU, only shows the fraction part (0-1)
 */
long double getAMU(struct Statistics *currentStats);

/**
 * print the statistics
 * @param - *currentStats, point to current Statistics
 *
 */
void printStatistics(struct Statistics *currentStats);

#endif
