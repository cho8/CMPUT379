#include <stdio.h>
#include <stdlib.h>
#include "memlayout.h"

int main() {
  printf("Test memlayout.c start");
  int size = 10;
  int actualSize;
  struct memregion regions[size];
  
  actualSize = get_mem_layout(regions,size);

  int i;
  for (i=0; i<actualSize; i++) {
    printf("%08x - %08x %c \n", (unsigned int)regions[i].from, (unsigned int)regions[i].to, regions[i].mode);
  }
}