#include "memlayout.h"
#include <string.h>

// Should be done
// Modifies stack space by inflating one region through malloc

unsigned int PAGE_SIZE=0x100;

void inflate_stack_and_get_diff() {
  int size=100;
  struct memregion testregions[size];
  struct memregion diff[size];
  struct memregion othertest[size];
  
  int actual_size = get_mem_diff(testregions, size, diff);		//record changes in diff

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
}

int main(){

  int size = 30;
  struct memregion in_regions[size];
  struct memregion diff[size];


  printf("Get layout:\n");

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

  inflate_stack_and_get_diff();
  


  return 0;
}
