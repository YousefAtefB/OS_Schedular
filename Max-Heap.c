#include <stdio.h>
#include <stdlib.h>
#include "headers.h"

// struct represents a single node of the elements of the heap
typedef struct 
{
    int id;
    int data;       // we sort based on this
}heap_node;

// struct represents a max heap
typedef struct
{
    heap_node *Arr;
    int size;
} Max_Heap;

// this function create and initialize a new heap
Max_Heap* Max_Heap_init(int size)
{
    Max_Heap * heap=malloc(sizeof(Max_Heap));
    heap->size = 0;
    heap->Arr=malloc(sizeof(heap_node)*size);
    return heap;
}

// this function swap two elements inside a heap array
void Max_Heap_swap(Max_Heap* heap,int u,int v)
{
    heap_node temp=heap->Arr[u];
    heap->Arr[u]=heap->Arr[v];
    heap->Arr[v]=temp;
}

// this function heapifies upwards from a certain index in the heap array
void Max_Heap_heapup(Max_Heap *heap ,int indx)
{
    if(indx==0 || heap->Arr[(indx-1)/2].data > heap->Arr[indx].data)return;
    Max_Heap_swap(heap,indx,(indx-1)/2);
    Max_Heap_heapup(heap,(indx-1)/2);
}

// this function adds an element to the heap to the right position
void Max_Heap_add(Max_Heap *heap, heap_node num)
{
    heap->Arr[heap->size] = num;
    Max_Heap_heapup(heap,heap->size);
    heap->size++;
}

// this function heapifies downwards from a certain index in the heap array
void Max_Heap_heapdown(Max_Heap* heap,int indx)
{
    int left=2*indx+1;
    int right=2*indx+2;
    int largest=left;

    if(left>=heap->size)
        return;

    if(right<heap->size && heap->Arr[right].data>heap->Arr[left].data)
        largest=right;

    if(heap->Arr[indx].data>heap->Arr[largest].data)
        return;

    Max_Heap_swap(heap,indx,largest);

    Max_Heap_heapdown(heap,largest);
}

// this function returns the root of the heap (max element) and removes it from the heap
int Max_Heap_getmax(Max_Heap *heap)
{
    int id=heap->Arr[0].id;
    heap->size--;
    Max_Heap_swap(heap,0,heap->size);
    Max_Heap_heapdown(heap,0);
    return id;
}

// this function checks whether the heap is empty or not
bool Max_Heap_empty(Max_Heap*heap)
{
    return heap->size==0;
}



void main()
{
    Max_Heap*heap=Max_Heap_init(6);
    heap_node u;
    u.id=1;u.data=10;
    Max_Heap_add(heap,u);
    u.id=2;u.data=20;
    Max_Heap_add(heap,u);
    u.id=3;u.data=30;
    Max_Heap_add(heap,u);
    u.id=4;u.data=60;
    Max_Heap_add(heap,u);
    u.id=5;u.data=50;
    Max_Heap_add(heap,u);
    u.id=6;u.data=40;
    Max_Heap_add(heap,u);

    while(!Max_Heap_empty(heap))
    {
        printf("\n%d\n",Max_Heap_getmax(heap));
    }


}
