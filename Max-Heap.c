#include <stdio.h>
#include<stdlib.h>
typedef struct {
    int *Arr;
    int size;
}Max_Heap;

static void Max_Heap_init(Max_Heap *Heap){

Heap->size=0;
    
}


void AddtoHeap(Max_Heap *Heap,int num){
    Heap->Arr[Heap->size++]=num;

}

void Max_Heap_init2(Max_Heap *Heap,int arr[],int size){
    Heap->size=0;
    for (int i = 0; i < size; i++)
    {
        AddtoHeap(Heap,arr[i]);
    }
    
}



void swap(int *array,int i,int largest){
    int temp = array[i];
    array[i]=array[largest];
    array[largest]=array[i];
}
void Heapify(int *array,int size,int i){

    int largest = i;
    int leftchild=2*i+1;
    int Rightchild=2*i+2;
    if (array[leftchild]>array[largest])
    {
        largest = leftchild;
    }
    if (array[Rightchild]>array[largest])
    {
        largest = Rightchild;
    }
    
    swap(array,i,largest);
}

void MaxHeap(Max_Heap *Heap){
    for (int i = Heap->size-1; i >= 0; i--)
    {
     Heapify(Heap->Arr,Heap->size,i);
    }
}


void main(){
    Max_Heap Heap;
    int arr[6]={3,9,2,1,4,15};
    Max_Heap_init2(&Heap,arr,6);
    MaxHeap(&Heap);
    
    printf("%d",Heap.Arr[0]);
}
