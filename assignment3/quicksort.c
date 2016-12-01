#include <stdio.h>
#include <math.h>

void swap(int * a, int * b){
    /* Swaps the values of two int pointers
    */
    int temp = *a;
    *a = *b;
    *b = temp;       
}



int getMedian(int * A, begin, end){
    // TODO: write median-of-medians algorithm
    }




int inPlacePartitioning(int* A, int begin, int end){
    /* Takes an array, finds its median, and arranges
     elements less than it to the first half of the
     sequence, and elements larger than it to the
     second half. Returns pivot index.
    */
    int pivot = int (floor((begin + end) / 2));    // Replace with getMedian
    pivot = A[pivot];
    while(begin <= end){
        if(A[begin] >= pivot && A[end] < pivot){
            swap(A[begin], A[end]);
        }
        if(A[begin] < pivot) begin++;
        if(A[end] >= pivot) end--;
    }
    return begin;
}


void quicksort(int * A, int begin, int end){ 
    if (begin < end){
        int pivot = inPlacePartitioning(A, begin, end);
        quicksort(A, begin, pivot - 1);
        return quicksort(A, pivot + 1, end);    // Tail call for second half to keep O(log n)
        }       
    }