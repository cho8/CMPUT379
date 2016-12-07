#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "simulator.h"
/*
Quicksort referenced from:
https://www.programmingalgorithms.com/algorithm/quick-sort-iterative?lang=C
and modified to reference virtual memory space in simulator
*/

void process () {


  int i,j,x, temp, p;
  int N;

  printf("number of keys: ");
  scanf("%d", &N);
  printf("Sorting %1d keys\n", N);

  /*Init function params don't matter, but need to call init */
  init(0,0);

  /* Generate the sorting problem (just random numbers) */

  for (i=0; i < N; i++) put (i, lrand48 ());

  /* Sort the numbers */

	int* stack = (int*)malloc(sizeof(int) * N);
  int startIndex = 0;
	int endIndex = N - 1;
	int top = -1;

	stack[++top] = startIndex;
	stack[++top] = endIndex;

	while (top >= 0)
	{
		endIndex = stack[top--];
		startIndex = stack[top--];

    x = get(endIndex);
  	i = (startIndex - 1);
    temp = 0;

  	for (j = startIndex; j <= endIndex - 1; ++j)
  	{
  		if (get(j) <= x)
  		{
  			++i;
        //swap
        temp = get(i);
      	put(i, get(j));
      	put(j, temp);
  		}
  	}

    //swap
    temp = get(i+1);
    put(i+1, get(endIndex));
    put(endIndex, temp);
  	p=(i + 1);

		if (p - 1 > startIndex)
		{
			stack[++top] = startIndex;
			stack[++top] = p - 1;
		}

		if (p + 1 < endIndex)
		{
			stack[++top] = p + 1;
			stack[++top] = endIndex;
		}
	}

	free(stack);
  done();
}
