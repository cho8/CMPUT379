#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include "memlayout.h"

/* TODO
	- take care of segfaults from get_mem_layout
*/

int PAGE_SIZE=0x4000;
void init_layout(struct memregion *regions, int size){

  size = get_mem_layout(regions, 1);		//causes segfault
  size = get_mem_layout(regions, size);

  printf("This is the initial layout of the program memory:\n");

  int i = 0;
  for (i; i<size; i++) {
    printf("%p - %p %d \n", regions[i].from, regions[i].to, regions[i].mode);
  }

}


void change_layout(struct memregion *old_regions, int size, struct memregion *diff){

  unsigned int size_change = get_mem_diff(old_regions, size, diff, 1);		//get number of entries in new mem_region array
  size_change = get_mem_diff(old_regions, size, diff, size_change);	//record changes in diff

  printf("The program memory has been altered. These changes have occured: \n");
  int i;
  for (i=0; i<size_change; i++) {
    printf("%-10p - %-10p %d \n", diff[i].from, diff[i].to, diff[i].mode);
  }
}


int main(){


  int size;	//Size of initial mem_region array
  int * test;

  size = 100;	// DELETE BEFORE SUBMITING
  test = (int*)malloc(sizeof(int));	//Char pointer to change memory

  if(test == 0){
    printf("ERROR: Memory not allocated\n");
    return 1;
  }

  struct memregion *in_regions;
  struct memregion *diff;

  printf("Get layout:\n");
  init_layout(in_regions, size);

  //This is where we actually change memory space
  test = (int *)realloc(test, 2 * sizeof(int));

  change_layout(in_regions, diff, size);


  free(test);

  return 0;
}
