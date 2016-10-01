#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include "memlayout.h"

/* TODO
	- verify memory changes are occuring
*/


void init_layout(struct memregion *regions, int size){

//  size = get_mem_layout(regions, 1);		
//  size = get_mem_layout(regions, size);

  printf("This is the initial layout of the program memory:\n");

  int i = 0;
  for (i; i<size; i++) {
    printf("%p - %p %d \n", regions[i].from, regions[i].to, regions[i].mode);
  }

}


void change_layout(struct memregion *old_regions, int size_or, struct memregion *diff){

  printf("Inside change_layout\n");

  unsigned int size_change = 10;

//  unsigned int size_change = get_mem_diff(old_regions, size_or, diff, 1);	//get number of entries in new mem_region array
//  size_change = get_mem_diff(old_regions, size_or, diff, size_change);		//record changes in diff

  printf("The program memory has been altered. These changes have occured: \n");
  int i;
  for (i=0; i<size_change; i++) {
    printf("%p - %p %d \n", diff[i].from, diff[i].to, diff[i].mode);
  }
}


int main(){


  int * size = malloc(sizeof(int));	//Size of initial mem_region array
  int * test;
  struct memregion *in_regions;
  struct memregion *diff;

  *size = 20; // DELETE BEFORE SUBMITTING

  test = (int*)malloc(sizeof(int));	//Int pointer to change memory

  if(test == 0){
    printf("ERROR: Memory not allocated\n");
    return 1;
  }

  init_layout(in_regions, *size);

  printf("after init\n");

  //This is where we actually change memory space
  test = (int *)realloc(test, 2 * sizeof(int));

  printf("before change\n");
  change_layout(in_regions, *size, diff);

  free(test);
  free(size);

  return 0;
}