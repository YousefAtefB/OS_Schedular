#include<stdio.h>
#include<stdlib.h>

/////////////////////////////
struct process
{
    int id;
    int arrival;
    int runtime;
    int priority;
};
///////////////////////////
struct node
{
    struct process *proc;
    struct node *next;
};
/////////////////////////////
struct queue
{
    int length;
    struct node *front;
    struct node *tail;
};
/////////////////////////////
void initialize(struct queue *q)
{
    q->length = 0;
    q->front = NULL;
    q->tail = NULL;
    
}
/////////////////////////////
bool isempty(struct queue *q)
{
    return (q->length == 0);
}
/////////////////////////////
void enqueue(struct queue *q, struct process *p)
{
   
        struct node *temp = malloc(sizeof(struct node));
        temp->proc= malloc(sizeof(struct process));
        temp->proc->id=p->id;
        temp->proc->arrival=p->arrival;
        temp->proc->runtime=p->runtime;
        temp->proc->priority=p->priority;
        temp->next = NULL;
        if(q->length!=0)
        {
            q->tail->next = temp;
            q->tail = temp;
        }
        else
        {
            q->front = q->tail = temp;
        }
        q->length++;
}
/////////////////////////////
struct process* dequeue(struct queue *q)
{
    if(q->length!=0)
    {
    struct node *tmp=malloc(sizeof(struct node));
    tmp->proc= malloc(sizeof(struct process));
    struct process *p= malloc(sizeof(struct process));
    p->id= q->front->proc->id;
    p->arrival= q->front->proc->arrival;
    p->runtime= q->front->proc->runtime;
    p->priority= q->front->proc->priority;
    tmp = q->front;
    q->front = q->front->next;
    q->length--;
    free(tmp);
    return(p);
    }
    return NULL;
}
/////////////////////////////
struct process* peek(struct queue *q)
{
    if(!(isempty(q)))
    {
    struct process *p= malloc(sizeof(struct process));
    p->id= q->front->proc->id;
    p->arrival= q->front->proc->arrival;
    p->runtime= q->front->proc->runtime;
    p->priority= q->front->proc->priority;
    return(p);
    }
    return NULL;
}
//////////////////////////////
void display(struct node *head)
{
    if(head == NULL)
    {
        printf("NULL\n");
    }
    else
    {
        printf("process id %d arrivaltime %d runtime %d priority %d\n",
        head->proc->id,head->proc->arrival,head->proc->runtime,head->proc->priority);
        display(head->next);
    }
}
/////////////////////////////
