#ifndef SIMULATOR_H
#define SIMULATOR_H

// Simulator function interfaces
void process();
void init (int psize, int winsize);
void put (unsigned int address, int value);
int get (unsigned int address);
void done ();

// Helper function intrfaces
void checkWindowInterval();
void markAccessedPage(int address);
int hashFunction(int address);
Node_t* allocateNode(int address, int value);
void putValue(int address, int value);
int getValue(int address);
void printBitArray(unsigned long x);

// defs
typedef struct Node_t {
	int address;
	int value;
	struct Node_t* next;
} Node_t;


#endif
