#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "simulator.h"

/*
	TODO:
	init
	get
	put
	done
*/

// Global variables
int HASHKEY_DATA = 1024;

int page_size = 0;
int window_size = 0;

int window_interval_count = 0; // number of window intervals occurred
int pages_in_use = 0;		// number of pages that are flagged

int window_index = 0; // windex :) the last winsize number of memory references
int mem_refs = 0; 	// total number of memory refernces!

int workingset_history[128];	// history of page referenced at a given window interval
int pages[128];					// arrays of pages

// Constants
int HASHKEY = 1024;
int MAX_ADDRESS = 33554431;

typedef struct Node_t {
	int address;
	int value;
	struct Node_t* next;
} Node_t;

Node_t datahash[1024];

void process () {
	int N, i, j, l, t, min, f, k;

	printf("number of keys: ");
	scanf("%d", &N);
	printf("Sorting %1d keys\n", N);

	// ignore the simulator init, already done with bash
	// init (128, 1000);

	/* Generate the sorting problem (just random numbers) */

	for (i=0; i < N; i++) put (i, lrand48 ());

	/* Sort the numbers */

	for (i = 0; i < N-1; i++) {
		for (j = i+1, f = min = get (i), k = i; j < N; j++)
			if ((t = get (j)) < min) {
				k = j;
				min = t;
			}
			put (i, min);
			put (k, f);
		}
		done ();
}

// prototypes
void checkWindowInterval();
void markAccessedPage(int address);
int hashFunction(int address);

int main(int argc, char* argv[]) {

	// TODO modify this for the sim bash script calling the simulator
	// assume simulator compiles algorithm.c and simulator.c
	// then calls this main with two parameters
	// i.e. ./simulator <pagesize> <windowsize>

	if (argc != 3) {
		printf("Usage : %s <pagesize> <windowsize>\n", argv[0]);
	}
	page_size = atoi(argv[1]);
	window_size = atoi(argv[2]);
	//
	// // start process
	printf("Page size %d, Win size %d, Process start...\n", page_size, window_size);
	process();
	//
	//
	// return(0);
}

/*
 * Initializes the simulator for experiment.
 * psize : page size
 * winsize : window size
 * ! Assignment clarification: ignore psize and winsize for favor of sim bash script
*/
void init (int psize, int winsize) {
	// ignore psize and winsize

	// page_size = psize;
	// window_size = winsize;

	return;
}

/*
 * Called by process to store value in memory at address.
 * address : integer indexing a word in virtual mem (0 to 2^25 -1)
*/
void put (unsigned int address, int value) {

	// check where we are in the working set window
	checkWindowInterval();

	// put the value in the address
	// data_space[address] = value;

	// flag the page in use
	markAccessedPage(address);

}

/*
 * Called by process to fetch contents at indicated memory word and returns its
 * value.
 * address : integer indxing a word in virtual mem (0 to 2^25 -1)
*/
int get (unsigned int address) {

	int d;
	// working set history stuff
	checkWindowInterval();

	// get the value in the address
	// int d = data_space[address];

	// flag the page in use
	markAccessedPage(address);

	// if NULL, handled on the process' end
	return d;
}

/*
 *	Terminates experiment.
 *	When called, our simulator hould produce:
 *	-	history of working set size quantized at the intervals corresponding to
 *	  window size. A long sequence of numbers taken across execution of process at
 *		window-sized intervals reflecting the working set size at those intervals
 *	- Working set requirements of the process averaged over its execution time
 *	  (average working set size taken over all mem references)
 */
void done () {
	int sum = 0;

	// history of working set size
	printf("Working set history:\n");
	printf("Interval   Pages\n");
	int i;
	for (i=0; i < window_interval_count; i++) {
		printf("%7d %8d\n", i, workingset_history[i]);
		sum += workingset_history[i];
	}
	printf("\n");

	// average working set size
	// make the average a decimal
	double average = (double)sum/mem_refs;
	printf("Average working set size over execution time: ");
	printf("%.2f\n", average);


}

void checkWindowInterval() {
	// "execution" moves forward
	window_index++;
	mem_refs++;
	printf("Mem refs: %d, Windex: %d \n", mem_refs, window_index);


	// if "traversed" window_size memory references (i.e. at an interval)
	if (window_index > window_size) {
		// printf("Window shift\n");

		// add page count to history at this interval
		workingset_history[window_interval_count] = pages_in_use;
		// new window interval, new working set sta
		window_interval_count++;
		// the next window-size pages
		window_index = 0;
		pages_in_use = 0;
		int i;
		for (i=0; i<sizeof(pages); i++) {
			printf("%d",pages[i]);
			pages[i]=0;
		}
	}

}

void markAccessedPage(int address) {
	if (pages[address / page_size] == 0) {
		pages_in_use++;
		pages[address / page_size] =1;
	}
}

int hashFunction(int address) {
	int key = address % HASHKEY_DATA;
	return key;
}

Node_t* allocateNode(int key, int value) {
	Node_t *new_node = (Node_t*)malloc(sizeof(Node_t));
  new_node->value = value;
  new_node->next = NULL;
	return new_node;
}

void putValue(int address, int value) {

	Node_t* curr;	// pointer for traversal

	// access the hash table (get the list head)
	int address_key = hashFunction(address);
	Node_t* node = &datahash[address_key];

	// if the beginning node is not initialized, put value there
	if (node->next == NULL) {
		Node_t* nextnode = allocateNode(address, value);
		node->next = nextnode;
		return;
	}

	// Go to the beginning of the list
	node = node->next;
	// traverse the nodes
	while (node->address != address) {
		// if at end of linked list, add a new node with our value
		if (node->next == NULL) {
			Node_t* nextnode = allocateNode(address, value);
			node->next = nextnode;
			return;
		}
		// more entries in linked list, continue traversing
		node = node->next;
	}

	// found node with same address
	if (node->address == address) {	// double check
		node->value = value;
	} else {
		printf("Traverse error!\n");
	}
}

// int getValue(int address, int value) {
// 	Node_t node;
// 	int result;
//
// 	// access the hash table
// 	int addresskey = hashFunction(address);
// 	node = hash[key];
// 	// if that hash entry is not initialized, return
// 	// don't worry about NULL values
// 	if (node->value == NULL ) {
// 		return node->value;
// 	}
//
// 	// traverse the nodes
// 	while (node->address != address) {
// 		// if at end of linked list, return NULL
// 		if (node->next == NULL) {
// 			return NULL;
// 		}
// 		node = node->next;
// 	}
//
// 	// found node with same address
// 	if (node->address == address) {
// 		return node->value;
// 	} else {
// 		printf("Traverse error!\n");
// 		return NULL;
// 	}
// }
