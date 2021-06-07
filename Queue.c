#include<stdio.h>
#include<stdlib.h>
#include"headers.h"

typedef struct node node;
typedef struct queue queue;

//__________________________________________________
struct node
{
    int data;
    node *nxt;
};

void node_init(node* n,int x)
{
    n->data=x;
    n->nxt=NULL;
}

//__________________________________________________


struct queue
{
    node *front, *rear;
};

queue* queue_init()
{
    queue *q =malloc(sizeof(queue));
    q->front=q->rear=NULL;
    return q;
}

void queue_push(queue *q,int x)
{
    node *temp=malloc(sizeof(node));
    node_init(temp,x);

    if(q->rear==NULL)
    {
        q->front=q->rear=temp;
        return;
    }

    q->rear->nxt=temp;
    q->rear=temp;
}

node* queue_pop(queue *q)
{
    node *temp=q->front;
    q->front=q->front->nxt;

    if(q->front==NULL)
        q->rear=NULL;

    free(temp);
}

int queue_front(queue * q)
{
    return q->front->data;
}

bool queue_empty(queue * q)
{
    return q->rear==NULL;
}

//this function free the queue
void queue_destroy(queue*q)
{
    free(q);
}

int main()
{
    queue *q = queue_init();
    queue_push(q,1);
    queue_push(q,44);
    queue_push(q,12);
    queue_push(q,71);

    for(int i=0;i<4;i++)
    {
        printf("%d\n",queue_front(q));
        queue_pop(q);
    }

    free(q);

}