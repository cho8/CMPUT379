#include "memlayout.h"
#include <sys/mman.h>
/* TODO
	- operations change RW permissions but change size (they should be constant)
 */

unsigned int PAGE_SIZE=0x100;

int main(){
	int *buffer;
	int size = 100;
	int actual_size_change;
	struct memregion in_regions[size];
	struct memregion diff[size];
        int i;

	/* Allocate a buffer aligned on a page boundary;
                initial protection is PROT_READ | PROT_WRITE */
	printf("Allocating buffer\n");
	buffer = (int*) calloc (1000, sizeof(int));
	int actual_size = get_mem_layout(in_regions, size);

	printf("Initial layout of program memory:\n");
	for (i=0; i<actual_size; i++) {
		printf("%-10p - %-10p", in_regions[i].from, in_regions[i].to);
		switch (in_regions[i].mode) {
		case MEM_NO:
			printf(" %s \n", "NO");
			break;
		case MEM_RW:
			printf(" %s \n", "RW");
			break;
		case MEM_RO:
			printf(" %s \n", "RO");
		}
	}
	printf("Number of memory regions: %d\n", actual_size);
	
	// mprotect(buffer, 1000*sizeof(int), PROT_NONE);
	actual_size_change = get_mem_diff(in_regions, actual_size, diff,size);		//record changes in diff

	printf("Program memory has altered. Memory regions changed: \n");
	int j;
	for (j=0; j<actual_size_change; j++) {
		printf("%-10p - %-10p", diff[j].from, diff[j].to);
		switch (diff[j].mode) {
		case MEM_NO:
			printf(" %s \n", "NO");
			break;
		case MEM_RW:
			printf(" %s \n", "RW");
			break;
		case MEM_RO:
			printf(" %s \n", "RO");
		}
	}
	printf("Number of changed regions: %d\n", actual_size_change);

	return 0;
}

