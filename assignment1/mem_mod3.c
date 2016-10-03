#include "memlayout.h"
#include <sys/mman.h>
/* TODO
	- operations change RW permissions but change size (they should be constant)
 */

unsigned int PAGE_SIZE=0x100;
void init_layout(struct memregion *regions){
	// removed paramter 'size' because not used

	// called twice to get whole memory layout
	int size = get_mem_layout(regions, 1);
	size = get_mem_layout(regions, size);


	printf("Initial layout of program memory:\n");

	int i;
	for (i=0; i<size; i++) {
		printf("%-10p - %-10p", regions[i].from, regions[i].to);
		switch (regions[i].mode) {
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
	printf("Actual size: %d\n", size);
}




void change_layout(struct memregion *old_regions, int size_or, struct memregion *diff){

	int size_change = get_mem_diff(old_regions, size_or, diff, 1);	//get number of entries in new mem_region array
	int actual_size_change = get_mem_diff(old_regions, size_or, diff, size_change);		//record changes in diff

	printf("Memory regions changed: \n");
	int i;
	for (i=0; i<actual_size_change; i++) {
		printf("%-10p - %-10p", diff[i].from, diff[i].to);
		switch (diff[i].mode) {
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
	printf("Actual size change: %d\n", actual_size_change);
}


int main(){
	int *buffer;
	int size = 100;
	struct memregion in_regions[size];
	struct memregion diff[size];


	/* Allocate a buffer aligned on a page boundary;
                initial protection is PROT_READ | PROT_WRITE */
	printf("Allocating buffer aligned on page boundary\n");

	buffer = (int*)memalign( PAGE_SIZE, 4 * PAGE_SIZE);

	int actual_size = get_mem_layout(in_regions, size);

	printf("Initial layout of program memory:\n");

	int i;
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



	printf("mprotect\n");
	mprotect(buffer, 4*PAGE_SIZE, PROT_READ);

	int actual_size_change = get_mem_diff(in_regions, size, diff);		//record changes in diff

	printf("Program memory has altered. Memory regions changed: \n");
	int i;
	for (i=0; i<actual_size_change; i++) {
		printf("%-10p - %-10p", diff[i].from, diff[i].to);
		switch (diff[i].mode) {
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

