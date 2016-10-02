#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include "memlayout.h"

/* TODO
	- verify memory changes are occuring
*/

unsigned int PAGE_SIZE=0x4000;
void init_layout(struct memregion *regions, int size){


  size = get_mem_layout(regions, 1);		// why is size passed in, then reassigned immediately?
  int actualSize = get_mem_layout(regions, size);

  // in the case where the actual size is larger than the number of array entries,
  int useSize = actualSize;
  //if (actualSize>size) {
	 // useSize = size;
  //}


  printf("This is the initial layout of the program memory:\n");

  int i;
  for (i=0; i<useSize; i++) {
    printf("%d %p - %p %d \n", i, regions[i].from, regions[i].to, regions[i].mode);
  }
  printf("Actual size: %d\n", actualSize);

}




void change_layout(struct memregion *old_regions, int size_or, struct memregion *diff){

  printf("Inside change_layout\n");


  int size_change = get_mem_diff(old_regions, size_or, diff, 1);	//get number of entries in new mem_region array
  int actual_size_change = get_mem_diff(old_regions, size_or, diff, size_change);		//record changes in diff
  int use_size = actual_size_change;

  // in the case where the actual size is larger than the number of array entries,
  if (actual_size_change>size_change) {
	  use_size = size_change;
  }
  printf("The program memory has been altered. These changes have occured: \n");
  int i;
  for (i=0; i<use_size; i++) {
    printf("%-10p - %-10p %d \n", diff[i].from, diff[i].to, diff[i].mode);
  }
  printf("Actual size change: %d", actual_size_change);
}


int main(){

  int size = 30;	//Size of initial mem_region array
  int * test;
  struct memregion in_regions[30];
  struct memregion diff[30];

  test = (int*)malloc(sizeof(int));	//Int pointer to change memory

  if(test == 0){
    printf("ERROR: Memory not allocated\n");
    return 1;
  }

  printf("Get layout:\n");

  init_layout(in_regions, size);

  printf("after init\n");

  //This is where we actually change memory space
  test = (int *)realloc(test, 2 * sizeof(int));


  printf("before change\n");
  change_layout(in_regions, size, diff);

  free(test);

  return 0;
}
