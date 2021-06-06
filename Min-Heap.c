#include <stdio.h>
#include <stdlib.h>
#include "headers.h"

// struct represents a single node of the elements of the heap
typedef struct 
{
    int id;
    int data;       // we sort based on this
}heap_node;

// struct represents a Min heap
typedef struct
{
    heap_node *Arr;
    int size;
} Min_Heap;

// this function create and initialize a new heap
Min_Heap* Min_Heap_init(int size)
{
    Min_Heap * heap=malloc(sizeof(Min_Heap));
    heap->size = 0;
    heap->Arr=malloc(sizeof(heap_node)*size);
    return heap;
}

// this function swap two elements inside a heap array
void Min_Heap_swap(Min_Heap* heap,int u,int v)
{
    heap_node temp=heap->Arr[u];
    heap->Arr[u]=heap->Arr[v];
    heap->Arr[v]=temp;
}

// this function heapifies upwards from a certain index in the heap array
void Min_Heap_heapup(Min_Heap *heap ,int indx)
{
    if(indx==0 || heap->Arr[(indx-1)/2].data < heap->Arr[indx].data)return;
    Min_Heap_swap(heap,indx,(indx-1)/2);
    Min_Heap_heapup(heap,(indx-1)/2);
}

// this function adds an element to the heap to the right position
void Min_Heap_add(Min_Heap *heap, int id,int data)
{
    heap_node element;
    element.id=id;element.data=data;
    heap->Arr[heap->size] = element;
    Min_Heap_heapup(heap,heap->size);
    heap->size++;
}

// this function heapifies downwards from a certain index in the heap array
void Min_Heap_heapdown(Min_Heap* heap,int indx)
{
    int left=2*indx+1;
    int right=2*indx+2;
    int smallest=left;

    if(left>=heap->size)
        return;

    if(right<heap->size && heap->Arr[right].data < heap->Arr[left].data)
        smallest=right;

    if(heap->Arr[indx].data < heap->Arr[smallest].data)
        return;

    Min_Heap_swap(heap,indx,smallest);

    Min_Heap_heapdown(heap,smallest);
}

// this function returns the root of the heap (Min element) and removes it from the heap
int Min_Heap_getMin(Min_Heap *heap)
{
    int id=heap->Arr[0].id;
    heap->size--;
    Min_Heap_swap(heap,0,heap->size);
    Min_Heap_heapdown(heap,0);
    return id;
}

// this function checks whether the heap is empty or not
bool Min_Heap_empty(Min_Heap*heap)
{
    return heap->size==0;
}

// this function free the heap
void Min_Heap_destroy(Min_Heap*heap)
{
    free(heap->Arr);
    free(heap);
}


void main()
{
    Min_Heap*heap=Min_Heap_init(6);

    Min_Heap_add(heap,1,10);
    Min_Heap_add(heap,2,20);
    Min_Heap_add(heap,3,40);
    Min_Heap_add(heap,4,50);
    Min_Heap_add(heap,5,30);
    Min_Heap_add(heap,6,60);


    while(!Min_Heap_empty(heap))
    {
        printf("\n%d\n",Min_Heap_getMin(heap));
    }

}
