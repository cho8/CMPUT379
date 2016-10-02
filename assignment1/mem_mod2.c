#include "memlayout.h"

/* TODO
	- verify memory changes are occuring
*/

unsigned int PAGE_SIZE=0x4000;
void init_layout(struct memregion *regions){
  // removed paramter 'size' because not used

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

  printf("Inside change_layout\n");


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
  printf("Actual size change: %d", actual_size_change);
}


int main(){

  unsigned int * test1;
  unsigned int * test2;
  int size = 30;
  struct memregion in_regions[size];
  struct memregion diff[size];

  test1 = (int*)malloc(sizeof(int));	//Int pointer to change memory
  test2 = (unsigned int*) malloc (40*sizeof(int));
  if(test1 == 0){
    printf("ERROR: Memory not allocated\n");
    return 1;
  }

  printf("Get layout:\n");

  init_layout(in_regions);

  printf("after init\n");

  //This is where we actually change memory space
  test1 = (unsigned int *)realloc(test1, 20000 * sizeof(int));
  test2 = (unsigned int *)calloc( 50000, 2* sizeof(int));


  printf("before change\n");
  change_layout(in_regions, size, diff);

  free(test1);
  free(test2);

  return 0;
}
