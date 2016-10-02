#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include "memlayout.h"

/* TODO
	- take care of segfaults from get_mem_layout
*/

#define RWtoRO "RW_RO_file"
#define ROtoNO "RO_NO_file"


void begin_file_ops(FILE * file1, FILE * file2){
  file1 = fopen(RWtoRO, "w"); 
  file2 = fopen(ROtoNO, "w");

  fclose(file2);
  file2 = fopen(ROtoNO, "r");
}



void do_file_ops(FILE * file1, FILE * file2){	//closes files which were previously opened, then reopens them in a new mode
  printf("inside do_file_ops\n");

  if(file1 != NULL){
    printf("file1 actually points somewhere \n");
  }

//  fclose(file1);
  
  printf("file1 has been closed\n");

  fclose(file2);

  file1 = fopen(RWtoRO, "r");	// file2 should have NO access
}



void init_layout(struct memregion *regions, int size){

//  size = get_mem_layout(regions, 1);		//causes segfault
//  size = get_mem_layout(regions, size);

  printf("This is the initial layout of the program memory:\n");

  int i = 0;
  for (i; i<size; i++) {
    printf("%p - %p %d \n", regions[i].from, regions[i].to, regions[i].mode);
  }

}




void change_layout(struct memregion *old_regions, struct memregion *diff){

  unsigned int size_change = 10;

//  unsigned int size_change = get_mem_diff(old_regions, size, diff, 1);		//get number of entries in new mem_region array
//  size_change = get_mem_diff(old_regions, size, diff, size_change);	//record changes in diff

  printf("The program memory has been altered. These changes have occured: \n");
  int i;
  for (i=0; i<size_change; i++) {
    printf("%p - %p %d \n", diff[i].from, diff[i].to, diff[i].mode);
  }
}


int main(){

  FILE * file1;
  FILE * file2;
  int *size = (int *)malloc(sizeof(int));	//Size of initial mem_region array
  struct memregion *in_regions;
  struct memregion *diff;

  *size = 10;	// DELETE BEFORE SUBMITING

  printf("before begin_file_ops\n");
  begin_file_ops(file1, file2);
  printf("after begin_file_ops\n");

  init_layout(in_regions, *size);
  printf("after init_layout\n");

  if(file1 != NULL){
    printf("before closing file1 (which is not null)\n");
    fclose(file1);
    printf("before do_file_ops\n");
  } else {
    printf("file1 is null in main\n");
  }

  printf("before do_file_ops \n");
  do_file_ops(file1, file2);
  printf("after do_file_ops\n");

  change_layout(in_regions, diff);
  printf("after change_layout\n");



  if(fclose(file1) != 0 || fclose(file2) != 0){
    printf("ERROR: file was not properly closed\n");
    return 1;
  }

  free(size);

  return 0;
}