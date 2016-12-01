#include <stdio.h>
#include <stdlib.h>

#include "simulator.h"



// The naive example in assigment spec
void process () {
	int N, i, j, l, t, min, f, k;

	printf("number of keys: ");
	scanf("%d", &N);
	printf("Sorting %1d keys\n", N);

	// ignore the simulator init, already done with bash
	// init (128, 1000);

	/* Generate the sorting problem (just random numbers) */

	for (i=0; i < N; i++) put (i, lrand48 ());

	/* Sort the numbers */

	for (i = 0; i < N-1; i++) {
		for (j = i+1, f = min = get (i), k = i; j < N; j++)
			if ((t = get (j)) < min) {
				k = j;
				min = t;
			}
			put (i, min);
			put (k, f);
		}
		done ();
}
