#include <stdio.h>
#include <math.h>

void swap(int * a, int * b){
    /* Swaps the values of two int pointers
    */
    int temp = *a;
    *a = *b;
    *b = temp;       
}


//      BEGIN MEDIAN OF MEDIANS
int getMedian(int *A, int size, int k) {
    /* Selects the kth element in an array A
    with 'size' number of elements. Will be used
    to return the median of A.
    */
    if (size == 1 && k == 0) return A[0];

    int m = int (size + 4)/5;              // m = number of bins of 5 element subarrays
    int *medians = new int[m];
    for (int i=0; i<m; i++) {
        if (5*i + 4 < size) {
            int *w = A + 5*i;
            for (int j0=0; j0<3; j0++) {
                int jmin = j0;
                for (int j=j0+1; j<5; j++) {
                    if (w[j] < w[jmin]) jmin = j;
                }
                swap(w[j0], w[jmin]);
            }
            medians[i] = w[2];
        } else {
            medians[i] = A[5*i];
        }
    }

    int pivot = getMedian(medians, m, m/2);
    delete [] medians;

    for (int i=0; i<size; i++) {
        if (A[i] == pivot) {
            swap(A[i], A[size-1]);
            break;
        }
    }

    int store = 0;
    for (int i=0; i<size-1; i++) {
        if (A[i] < pivot) {
            swap(A[i], A[store++]);
        }
    }
    swap(A[store], A[size-1]);

    if (store == k) {
        return pivot;
    } else if (store > k) {
        return getMedian(A, store, k);
    } else {
        return getMedian(v+store+1, size-store-1, k-store-1);
    }
}

//      END MEDIAN-OF-MEDIANS



int inPlacePartitioning(int* A, int begin, int end){
    /* Takes an array, finds its median, and arranges
     elements less than it to the first half of the
     sequence, and elements larger than it to the
     second half. Returns pivot index.
    */
    int pivot = int (floor((begin + end) / 2));    // Replace with getMedian
    pivot = getMedian(A, end-begin+1, pivot);
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