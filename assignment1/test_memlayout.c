#include "memlayout.h"

unsigned int PAGE_SIZE = 0x1000;

int main() {
  printf("=== Test memlayout.c start ===\n");

  unsigned int size = 100;
  unsigned int actual_size;
  unsigned int useSize = size;
  unsigned int actual_diffSize;
  struct memregion regions[size];

  printf("Test get_mem_layout\n");
  actual_size = get_mem_layout(regions,size);


  if (actual_size < size) {
	  useSize=actual_size;
  }

  int i;
  for (i=0; i< useSize; i++) {
    printf("%-10p - %-10p %d \n", regions[i].from, regions[i].to, regions[i].mode);
  }
  printf("Actual number of regions: %d\n", actual_size);

  printf("=== TEST get_mem_diff ===\n");

  struct memregion thediff[size];

  int *test;
  test = (int*) calloc(1000, sizeof(int));

  actual_diffSize = get_mem_diff(regions, size, thediff, size);
  useSize=size;
  if (actual_diffSize < size) {
	  useSize=actual_diffSize;
  }
  for (i=0; i< useSize; i++) {
    printf("%-10p - %-10p %d \n", thediff[i].from, thediff[i].to, thediff[i].mode);
  }
  printf("Actual number of different regions: %d\n", actual_diffSize);



}
