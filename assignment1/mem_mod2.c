#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include "memlayout.h"

/* TODO
	- take care of segfaults from get_mem_layout
*/


void init_layout(struct memregion *regions, int size){

//  size = get_mem_layout(regions, 1);		//causes segfault
//  size = get_mem_layout(regions, size);

  printf("This is the initial layout of the program memory:\n");

  int i = 0;
  for (i; i<size; i++) {
    printf("%p - %p %d \n", regions[i].from, regions[i].to, regions[i].mode);
  }

}

/*
void change_layout(struct memregion *old_regions, struct memregion *diff){

  unsigned int size_change = get_mem_diff(old_regions, size, diff, 1);		//get number of entries in new mem_region array
  size_change = get_mem_diff(old_regions, size, diff, size_change);	//record changes in diff

  printf("The program memory has been altered. These changes have occured: \n");
  int i;
  for (i=0, i<size_change, i++) {
    printf("$s - $s %d \n", diff[i].from, diff[i].to, diff[i].mode);
  }
}
*/

int main(){


  int *size = (int *)malloc(sizeof(int));	//Size of initial mem_region array
  int * test1 = (int*)malloc(sizeof(int));
  int * test2 = (int*)malloc(sizeof(int));

  struct memregion *in_regions;
  struct memregion *diff;

  *size = 10;	// DELETE BEFORE SUBMITING

  if(test1 == NULL || test2 == NULL){
    printf("ERROR: Memory not allocated\n");
    return 1;
  }

  init_layout(in_regions, *size);


  //This is where we actually change memory space
  test1 = (int *)realloc(test1, 2 * sizeof(int));

  if(test1 == NULL || test2 == NULL){
    printf("ERROR: Faulty memory alteration\n");
    return 1;
  }

//  change_layout(in_regions, diff);

  free(size);
  free(test1);
  free(test2);

  return 0;
}