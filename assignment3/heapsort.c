#include <stdio.h>
#include <math.h>

void swap(int * a, int * b){
    /* Swaps the values of two int pointers
    */
    int temp = *a;
    *a = *b;
    *b = temp;       
}


int LeftChild(int node){
    return (2*node + 1);
    }
    
int RightChild(int node){
    return(2*node +2);
    }
    
int Parent(int node){
    return int(floor((node-1)/2));
    }




void siftDown(int * A, int start, int end){
    int root = start;
    int child, swap;
    
    while(LeftChild(root) <= end){      // while root has atleast one child
        child = LeftChild(root);
        swap = root;                    // Swap keeps track of child to swap with
        
        if(A[swap] < A[child]){
            swap = child;
            }
        
        if((child+1)<= end && A[swap] < A[child+1]){    //If there is a bigger right child
            swap = child + 1;
            }
        if (swap == root){      //Root holds largest element. Assuming subheaps rooted at children
            return;             //are valid, we are done.
            }
        else{
            swap(A[root], A[swap]);
            root = swap;
            }
        }
    }
    
    

void heapify(int * A, length){
    /* Used the wikipedia page on heapsort for reference.
    We are implementing a heap structure in the input array.
    */
    
    int start = Parent(length);    // start <- Parent node of node (length-1)
    
    while(start >= 0){
        siftDown(A, start, length-1);
        start--;
        }
    }




void heapsort(int * A, int length){
    heapify(A, length);
    int end = length - 1;
    
    while(end > 0){
        swap(A[end], A[0]);
        end--;
        siftDown(A, 0, end);
        }
    }