#
# Makefile
#
# 
#
#

CC = gcc
CFLAGS = -g -Wall -pedantic -pg
OBJECTSFIFO = TraceParser.o DiskQueue.o InvertedPageTable.o PageTable.o Process.o Statistics.o FifoReplacement.o
OBJECTSLRU = TraceParser.o DiskQueue.o InvertedPageTable.o PageTable.o Process.o Statistics.o LRUReplacement.o
OBJECTSCLOCK = TraceParser.o DiskQueue.o InvertedPageTable.o PageTable.o Process.o Statistics.o ClockReplacement.o
OBJECTS = TraceParser.o DiskQueue.o InvertedPageTable.o PageTable.o Process.o Statistics.o 

537pfsim-fifo: $(OBJECTSFIFO) FifoReplacement.o
	$(CC) $(CFLAGS) -o 537pfsim-fifo $(OBJECTSFIFO) 

537pfsim-lru: $(OBJECTSLRU) LRUReplacement.o
	$(CC) $(CFLAGS) -o 537pfsim-lru $(OBJECTSLRU) 

537pfsim-clock: $(OBJECTSCLOCK) ClockReplacement.o
	$(CC) $(CFLAGS) -o 537pfsim-clock $(OBJECTSCLOCK) 

TraceParser.o: TraceParser.c Process.h DiskQueue.h InvertedPageTable.h PageTable.h PageReplacementAlgorithm.h Statistics.h 
	$(CC) $(CFLAGS) -c TraceParser.c

DiskQueue.o: DiskQueue.c DiskQueue.h
	$(CC) $(CFLAGS) -c DiskQueue.c

InvertedPageTable.o: InvertedPageTable.c InvertedPageTable.h
	$(CC) $(CFLAGS) -c InvertedPageTable.c

PageTable.o: PageTable.c PageTable.h
	$(CC) $(CFLAGS) -c PageTable.c

Process.o: Process.c Process.h PageTable.h
	$(CC) $(CFLAGS) -c Process.c

Statistics.o: Statistics.c Statistics.h
	$(CC) $(CFLAGS) -c Statistics.c

# page replacement algorithms below

FifoReplacement.o: FifoReplacement.c PageReplacementAlgorithm.h
	$(CC) $(CFLAGS) -c FifoReplacement.c

LRUReplacement.o: LRUReplacement.c PageReplacementAlgorithm.h
	$(CC) $(CFLAGS) -c LRUReplacement.c

ClockReplacement.o: ClockReplacement.c PageReplacementAlgorithm.h
	$(CC) $(CFLAGS) -c ClockReplacement.c

clean:
	rm -f $(OBJECTS) FifoReplacement.o LRUReplacement.o ClockReplacement.o 537pfsim-fifo 537pfsim-lru 537pfsim-clock


