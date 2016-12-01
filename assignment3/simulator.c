#include "simulator.h"

/*
	TODO:
	init
	get
	put
	done
*/

// Global variables
int page_size = 0;
int window_size = 0;

int window_interval_count = 0;
int pages_in_use = 0;		// number of pages that are flagged

int window_index = 0; // windex :3

int workingset_history[128];
int pages[128];					// arrays of pages

int data_space[1024];	// TODO Change this magic number later?

int main(int argc, char* argv[]) {

	// TODO modify this for the sim bash script calling the simulator
	// assume simulator compiles algorithm.c and simulator.c
	// then calls this main with two parameters
	// i.e. ./simulator <pagesize> <windowsize>

	if (argc != 3) {
		printf("Usage : %s <pagesize> <windowsize>\n", argv[0]);
	}
	psize = atoi(argv[1]);
	winsize = atoi(argv[2]);
	//
	// // start process
	// process();
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
	// assert(psize > 0);
	// assert(winsize > 0);
	//
	// page_size = psize;
	// window_size = winsize;



	return;
}

/*
 * Called by process to store value in memory at address.
 * address : integer indexing a word in virtual mem (0 to 2^25 -1)
*/
void put (unsigned int address, int value) {


	// traverse window
	accesses++;
	if (accesses > window_size) {
		accesses = 0;	//
		// add page count to history
		workingset_history[window_interval_count] = page_count;
		// window shifts over, next intervals
		window_interval_count++;
		// new window, new page count!
		page_count = 0;
	}

	// put the value in the address
	data_space[address]

	// flag the page in use
	if (pages[address % PAGE_SIZE] == 0) {
		page_count++;
	}



}

/*
 * Called by process tto fetch contents at indicated memory word and returns its
 * value.
 * address : integer indxing a word in virtual mem (0 to 2^25 -1)
*/
int get (unsigned int address) {

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

}
