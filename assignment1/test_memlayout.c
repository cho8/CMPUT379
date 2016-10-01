#include "memlayout.h"

int PAGE_SIZE = 0x2000;

int main() {
  printf("=== Test memlayout.c start ===\n");

  unsigned int size = 100;
  unsigned int actualSize;
  unsigned int useSize = size;
  unsigned int actualDiffSize;
  struct memregion regions[size];

  printf("Test get_mem_layout\n");
  actualSize = get_mem_layout(regions,size);


  if (actualSize < size) {
	  useSize=actualSize;
  }

  int i;
  for (i=0; i< useSize; i++) {
    printf("%-10p - %-10p %d \n", regions[i].from, regions[i].to, regions[i].mode);
  }
  printf("Actual number of regions: %d\n", actualSize);

  printf("=== TEST get_mem_diff ===\n");

  struct memregion thediff[size];

  int *test;
  test = (int*)malloc(sizeof(int));	//Char pointer to change memory
  test = (int *)realloc(test, 2 * sizeof(int));   //This is where we actually change memory space

  actualDiffSize = get_mem_diff(regions, size, thediff, size);
  useSize=size;
  if (actualDiffSize < size) {
	  useSize=actualDiffSize;
  }
  for (i=0; i< useSize; i++) {
    printf("%-10p - %-10p %d \n", thediff[i].from, thediff[i].to, thediff[i].mode);
  }
  printf("Actual number of different regions: %d\n", actualDiffSize);

}
