#include "memlayout.h"
#include <string.h>

// Should be done
// Modifies stack space by inflating one region through malloc

unsigned int PAGE_SIZE=0x1000;
void init_layout(struct memregion *regions){

  // called twice to get whole memory layout
  int size = get_mem_layout(regions, 1);
  int actual_size = get_mem_layout(regions, size);

  printf("This is the initial layout of the program memory:\n");

  printf("Initial layout of program memory:\n");

  int i;
  for (i=0; i<actual_size; i++) {
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
  printf("Actual size change: %d", actual_size_change);
}

void inflate_stack_and_get_diff() {
  int size=10000;
  struct memregion testregions[size];
  struct memregion diff[size];
  struct memregion othertest[size];
  
  change_layout(testregions, size, diff);
}

int main(){

  int size = 30;
  struct memregion in_regions[size];
  struct memregion diff[size];


  printf("Get layout:\n");

  init_layout(in_regions);

  inflate_stack_and_get_diff();
  


  return 0;
}
