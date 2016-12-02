#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "simulator.h"


// Constants
const int HASHKEY = 1024;
const int MAX_ADDRESS = 33554431;  // max address is 2^25-1
const int HISTORY_SIZE = 256;

// globals
int page_size = 0;
int window_size = 0;

int window_interval_count = 0;	// number of window intervals occurred
int pages_in_use = 0;						// number of pages that are flagged

int window_index = 0; 					// windex :) the last winsize number of memory references
int mem_refs = 0; 							// total number of memory references
int workingset_sum;							// sum of working set size over all mem_refs

int history_size = 0;						// Size of history
int n_pages = 0;								// Number of pages in memory
int *workingset_history;		// working set size history, default size 256
int workingset_extension = 1;			// dynamically lengthen the history if needed
unsigned long pages;						// bit array of pages

Node_t* datahash;								// hash table representing address space



int main(int argc, char* argv[]) {


	if (argc != 3) {
		printf("Usage : %s <pagesize> <windowsize>\n", argv[0]);
	}
	page_size = atoi(argv[1]);
	window_size = atoi(argv[2]);

	printf("Page size %d, Win size %d, Process start...\n", page_size, window_size);

	// start process
	process();

}

/*
 * Initializes the simulator for experiment.
 * psize : page size
 * winsize : window size
 * ! Assignment clarification: ignore psize and winsize for favor of sim bash script
*/
void init (int psize, int winsize) {
	// ignore psize and winsize

	// initialize page mapping
	n_pages = MAX_ADDRESS+1 / page_size;
	pages = 0x00;

	// initialize memory hash
	datahash = (Node_t*) malloc(HASHKEY*(sizeof(Node_t)));

	// initialize working set history record
	workingset_history = (int*) malloc(workingset_extension * HISTORY_SIZE* (sizeof(int)));

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
	putValue(address,value);

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
	d = getValue(address);

	// flag the page in use
	markAccessedPage(address);

	// if NULL, handled on the process' end
	return d;
}

/*
 * Terminates experiment.
 * When called, our simulator hould produce:
 * - history of working set size quantized at the intervals corresponding to
 *   window size. A long sequence of numbers taken across execution of process at
 *   window-sized intervals reflecting the working set size at those intervals
 * - Working set requirements of the process averaged over its execution time
 *   (average working set size taken over all mem references)
 */
void done () {


	// history of working set size
	printf("Working set history:\n");
	printf("Interval   Pages\n");
	int i;
	for (i=0; i < window_interval_count; i++) {
		printf("%7d %8d\n", i, workingset_history[i]);
	}

	// average working set size
	// make the average a decimal
	double average = (double)workingset_sum/mem_refs;
	printf("Average working set size over execution time: ");
	printf("%.6f\n", average);


}

/*
 * Helper function for checking the window interval. If at the end of the interval,
 * reset the interval index and clear view of pages
 */
void checkWindowInterval() {
	// "execution" moves forward
	window_index++;
	mem_refs++;
	workingset_sum += pages_in_use;
	// printf("Mem refs: %d, Windex: %d \n", mem_refs, window_index);

	// if "traversed" window_size memory references (i.e. at an interval)
	if (window_index > window_size) {
		if (window_interval_count >= workingset_extension*HISTORY_SIZE) {
			// we need more room to store history
			workingset_extension++;
			workingset_history = (int*) realloc (workingset_history, workingset_extension*HISTORY_SIZE*sizeof(int));
		}

		// add page count to history at this interval
		workingset_history[window_interval_count] = pages_in_use;
		// new window interval, new working set window
		window_interval_count++;

		// the next window-size pages
		window_index = 0;
		pages_in_use = 0;

		// clear the pages referenced array
		pages = 0x0;
	}

}

/*
 * Helper function for flagging which page has been accessed
 */
void markAccessedPage(int address) {
	int page_no = address / page_size;
	unsigned long page_bit = (1<<page_no);

	// if page hasn't been visited, flag it and incr page_count;
	if ((pages & (1 << (page_no))) == 0) {
		pages_in_use++;
		pages = pages | (1 << (page_no));
	}

	// Print out the bit array for visual representation
	// printBitArray(page_bit);
	// printBitArray(pages);

}

/*
 * Hashes the address of for the hash table representation of the 2^25-1
 * address space.
 */
int hashFunction(int address) {
	int key = address % HASHKEY;
	return key;
}

/*
 * Allocate memory for a new node in the linked list of the hash table
 */
Node_t* allocateNode(int address, int value) {

	Node_t *new_node = (Node_t*)malloc(sizeof(Node_t));
	new_node->address = address;
  new_node->value = value;
  new_node->next = NULL;

	return new_node;
}

/*
 * Helper function for putting a value in our 2^25-1 memory representation.
 * Traverses the linked list pointed to by the hash table entry and updates value.
 * If value does not exist in the list, allocate a new node at the end.
 */
void putValue(int address, int value) {

	Node_t* node;
	Node_t* head;

	// access the hash table (get the list head)
	int address_key = hashFunction(address);
	head = &datahash[address_key];

	// if the beginning node is not initialized, put value there
	if (head->next  == NULL) {
		Node_t* newnode = allocateNode(address, value);
		head->next = newnode;
		return;
	}

	// Go to the beginning of the list
	node = head->next;
	// traverse the nodes
	while (node->address != address) {
		// if at end of linked list, add a new node with our value
		if (node->next == NULL) {
			node->next = allocateNode(address, value);
			return;
		}
		// more entries in linked list, continue traversing
		node = node->next;
	}

	// found node with same address
	if (node->address == address) {	// double check
		node->value = value;
	} else {
		printf("Segmentation fault!\n");
	}
}


/*
 * Helper function for getting a value from memory.
 * Traverses the linked list pointed to by the hash table entry and updates value.
 * If value does not exist, return "garbage"
 */
int getValue(int address) {

	Node_t* node;
	Node_t* head;

	// access the hash table (get the list head)
	int address_key = hashFunction(address);
	head = &datahash[address_key];

	// if that hash entry is not initialized, nothing is there
	// return null or garbage
	if (head->next ==  NULL ) {
		printf("Segmentation fault\n");
		return head->value;
	}

	// Go to the beginning of the list
	node = head->next;
	// traverse the nodes
	while (node->address != address) {
		// if at end of linked list, no value found
		if (node->next == NULL) {
			printf("Segmentation fault\n");
			return head->value;
		}
		// more entries in linked list, continue traversing
		node = node->next;
	}

	// found node with same address
	if (node->address == address) { // double check
		return node->value;
	} else {
		printf("Segmentation fault\n");
		return head->value;
	}
}

/*
 * Prints the 32-bit representation of a long
 */
void printBitArray(unsigned long x) {
	int bit = 0;

  // check bits of x for first 1
  for (int i = 31; i >= 0; i--) {
      bit = ((x >> i) & 1);
			printf("%d",bit);
  }
	printf("\n");

}
