#include "memlayout.h"

unsigned int PAGE_SIZE=4096;
void init_layout(struct memregion *regions){
  // removed parameter 'size' because not used

  // called twice to get whole memory layout
  int size = get_mem_layout(regions, 1);
  int actual_size = get_mem_layout(regions, size);

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
  printf("Actual size: %d\n", actual_size);

}




void change_layout(struct memregion *old_regions, int size_or, struct memregion *diff){


  int size_change = get_mem_diff(old_regions, size_or, diff, 1);	//get number of entries in new mem_region array
  int actual_size_change = get_mem_diff(old_regions, size_or, diff, size_change);		//record changes in diff

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
  printf("Actual size change: %d\n", actual_size_change);
}

int main(){
  int *test;
  int size = 30;
  struct memregion in_regions[size];
  struct memregion diff[size];


  //struct myStruct *struct1;
  printf("Get layout:\n");
  test = malloc(1000*sizeof(int));	
  init_layout(in_regions);	
  
  test = realloc(test, 200000*sizeof(int));
  free(test);
  change_layout(in_regions, size, diff);
  //free(struct1);

  
  return 0;
}
