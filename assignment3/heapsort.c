#include <stdio.h>
#include <stdlib.h>

#include "simulator.h"


 /*
 Heapsort implementation from:
 https://rosettacode.org/wiki/Sorting_algorithms/Heapsort
 Modified to use simulator put/get methods instead of int* pointers
 */
int max (int n, int i, int j, int k) {
    int m = i;
    if ((j < n) && (get(j) > get(m))) {
        m = j;
    }
    if ((k < n) && (get(k) > get(m))) {
        m = k;
    }
    return m;
}

void downheap (int n, int i) {
    while (1) {
        int j = max(n, i, 2 * i + 1, 2 * i + 2);
        if (j == i) {
            break;
        }
        int t = get(i);
        put(i, get(j));
        put(j,t);
        i = j;
    }
}

void _heapsort (int n) {
    int i;
    for (i = (n - 2) / 2; i >= 0; i--) {
        downheap(n, i);
    }
    for (i = 0; i < n; i++) {
        int t = get(n - i - 1);
        put(n - i - 1, get(0));
        put(0, t);
        downheap(n - i - 1, 0);
    }
}

void process () {
    int i;
    int N;

  	printf("number of keys: ");
  	scanf("%d", &N);
  	printf("Sorting %1d keys\n", N);

    /*Init function params don't matter, but need to call init */
    init(0,0);



  	/* Generate the sorting problem (just random numbers) */

  	for (i=0; i < N; i++) put (i, lrand48 ());

  	/* Sort the numbers */

    _heapsort(N);

    done();

}
