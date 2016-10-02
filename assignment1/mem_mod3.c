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

unsigned int PAGE_SIZE=0x64;

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


int main(){

  FILE * file1;
  FILE * file2;
  int size = 30;	//Size of initial mem_region array
  struct memregion in_regions[size];
  struct memregion diff[size];

  printf("before begin_file_ops\n");
  begin_file_ops(file1, file2);
  printf("after begin_file_ops\n");

  init_layout(in_regions);
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

  change_layout(in_regions, size, diff);
  printf("after change_layout\n");



  if(fclose(file1) != 0 || fclose(file2) != 0){
    printf("ERROR: file was not properly closed\n");
    return 1;
  }

  return 0;
}
