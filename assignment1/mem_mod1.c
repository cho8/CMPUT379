#include "memlayout.h"

// should be done
// allocates memory space on the heap

unsigned int PAGE_SIZE=0x100;

int main(){
	int size = 30;
	struct memregion in_regions[size];
	struct memregion diff[size];
	int actual_size;
	int actual_size_change;
	int i;
	actual_size = get_mem_layout(in_regions, size);

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
	sbrk(2*PAGE_SIZE);

	actual_size_change = get_mem_diff(in_regions, actual_size, diff, size);		//record changes in diff

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
