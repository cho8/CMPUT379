CMPUT379 Assignment 3

Included is a simulator that studies the working set sizes of programs running in memory. To demonstrate how the simulator works, two sorting algorithms (heapsort and quicksort) are included as well.

To run a simulation:

sh sim <pagesize> <windowsize> <sort-algorithm>

where <pagesize> is the page size, <windowsize> is the window size of the working set, and <sort-algorithm> is the name of the sorting algorithm (example, quicksort, heapsort).

The simulator will output the working set size history and calculate the average working set requirements over all memory references.

Please note:
If you are using another sorting algorithm you MUST call the simulator's init() function within process(), otherwise the working set history is not initialized.
