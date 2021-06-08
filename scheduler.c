#include "headers.h"

#include "string.h"
#include "datastructures/schedulerQueue.c"
#include "datastructures/Min-Heap.c"

#include <errno.h>

// algorithms
#define FCFS 1
#define SJF  2
#define HPF  3
#define SRTN 4
#define RR   5

// states
#define ARRIVED 0
#define STARTED 1
#define STOPPED 2
#define RESUMED 3
#define FINISHED 4

//this integer indicates the algorithm currently being used
int algo_typ;

//this integer is the id of the current running process
int cur_pro=-1;

//the quantum specifies for each process in round robin
int quantum=0;

//the time left for the current running process to be preempted (remaining quantum time)
int cur_pro_quantum=0;

// upper bound to the number of processes (recieved from the process_generator which in fact is the number of processes in the input file) 
int total_num_pro;

// queue to hold the id of the arrived processes in case of alogrithm type fcfs
queue *q;

// heap used as datastructure that holds the processes sorted based on some criteria
Min_Heap *heap;

// struct represent one element of the pcb
struct pcb_node
{
    int pid; 
    int arrival_time;
    int running_time;
    int priority;
    int remaining_time;
    // to avoid updating all the processes in the waiting list we can calculate this using  
    // waiting_time = (current clock-arrival_time)-(running_time-remaining_time)
    int Turn_around_time; 
    int waiting_time;  //we can make it only by Turn_around time - running time;
    int state;
};

typedef struct pcb_node pcb_node;

// the pcb
pcb_node **pcb;

// structs used to recieve element from the process generator through the msgqueue
struct process
{
    int id;
    int arrival;
    int runtime;
    int priority;
};

struct msgbuff
{
  long mtype;
  struct process p;
};

//the msg queue used to communicate with the generator
int msgqid;

// the .log file that is used to write in it the scheduling flow
FILE * log_out;

// indicates that all processes arrived
bool allarrived;

// keep track of number of processes in pcb
int total_in_pcb;

// OUTPUT NOTE: whenever you encounter ___________print___________  means we should output to file here




void initialize();
void arrived();
void schedule();
bool timestep();
void printstate(int id);
void clearResources(int signum);


int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);


    printf("\nscheduler: welcome to the scheduler %d\n",getpid());

    // reading the algorithm type from command line
    algo_typ=atoi(argv[1]);
    
    // reading the quantum used in algo 5
    if(algo_typ==5)
        quantum=atoi(argv[2]);
    // reading the total number of processes (upper bound to the number of processes)
    total_num_pro=algo_typ==5?atoi(argv[3]):atoi(argv[2]);

    printf("\nscheduler:algo_type=%d total_pro=%d\n",algo_typ,total_num_pro);

    initClk();

    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.

    initialize();

    while(!allarrived || total_in_pcb)
    {
        arrived();
       schedule();
       timestep();
    }

    printf("\nscheduler: closing allarriver=%d total_in_pcb=%d \n",allarrived,total_in_pcb);

    destroyClk(true);

}


//this function is responsible for initializing the scheduler, recieving the  algorithm type and the number of proccesses
void initialize()
{
    // initializing the out pcb
    pcb=malloc(sizeof(pcb_node*)*(total_num_pro+1));

    //getting the id of the msgqueue used to communicate with the generator
    msgqid = msgget(1, IPC_CREAT | 0666);

    printf("\nscheduler: msgqid=%d\n",msgqid);

    // initialize the blocks with null
    for(int i=0;i<=total_num_pro;i++)
        pcb[i]=NULL;


    // intialize the chosen algo
    switch(algo_typ)
    {
        case FCFS:
        case RR:
            // intializing a queue used in the algo 
            q=queue_init();
        break;
        
        case SJF:
        case HPF:
        case SRTN:
            // intializing a heap used in the algo
            heap=Min_Heap_init(total_num_pro);
        break;
    }

    // intializing the log file
    printf("\nscheduler: comment in log\n");
    log_out=fopen("scheduler.log","w");
    fprintf(log_out,"#At\ttime\tx\tprocess\ty\tstate\tarr\tw\ttotal\tz\tremain\ty\twait\tk\n");
    fflush(log_out);
}

// this function is responsible for recieving processes from the proccess_generator,fork it, store it in the pcb and put its id in the 
// datastructure of the algorithm
void arrived()
{
    // recieving processes
    while(true)
    {
        struct msgbuff message;
        errno=0;

        //getting a process from generator through msg queue 
        msgrcv(msgqid,&message,sizeof(message.p),0,IPC_NOWAIT);

        if(errno==ENOMSG)
            break;

        if(message.p.id==-1)
        {
            printf("\nscheduler:scheduler all arrived\n");
            allarrived=1;
            return;
        }

        printf("\nscheduler: recieved id=%d\n",message.p.id);

        total_in_pcb++;

        struct process temp=message.p;
        // making a new block for the process
        pcb[temp.id]=malloc(sizeof(pcb_node));

        // inserting the process into the pcb
        pcb[temp.id]->arrival_time=temp.arrival;
        pcb[temp.id]->running_time=temp.runtime;
        pcb[temp.id]->priority=temp.priority;
        pcb[temp.id]->remaining_time=temp.runtime;
        pcb[temp.id]->waiting_time=0;
        pcb[temp.id]->state=ARRIVED;

        printf("\nscheduler: print arrived\n");
        //___________print___________
        printstate(temp.id);


        printf("\nscheduler: forking\n");

        int pid=fork();
        if(pid==0)
        {
            char str_id[10],str_running_time[10];
            sprintf(str_id,"%d",temp.id);
            sprintf(str_running_time,"%d",temp.runtime);
            // swap the child with the process code and send it its id
            execl("./process.out","./process.out",str_id,str_running_time,NULL);
        }

        pcb[temp.id]->pid=pid;

        // stop the signal after forking it so it won't start right away
        kill(pid,SIGSTOP);

        // insert the process id inside the algorithm datastructure
        switch (algo_typ)
        {
            case FCFS:
            case RR:
                queue_push(q,temp.id);
            break;
            case SJF:
                //here we insert the id of the process and the data that we want to sort based on it 
                Min_Heap_add(heap,temp.id,temp.runtime);
            break;
            case HPF:
                //here we insert the id of the process and the data that we want to sort based on it 
                Min_Heap_add(heap,temp.id,temp.priority);
            break;
            case SRTN:
                //here we insert the id of the process and the data that we want to sort based on it 
                Min_Heap_add(heap,temp.id,temp.runtime);
            break;

        }
    }

}



// this function is responsible for scheduling the processes based on the chosen algo
void schedule()
{

    //check if current running process finished remove it and remove its ((block)) change its state
    if(cur_pro!=-1)
    {
        if(pcb[cur_pro]->remaining_time<=0)
        {    
            pcb[cur_pro]->state=FINISHED;
            //___________print___________
            printstate(cur_pro);
            free(pcb[cur_pro]);
            pcb[cur_pro]=NULL;
            cur_pro=-1;
            total_in_pcb--;
        }
    }

    switch (algo_typ)
    {
        case FCFS:
            // no current running process then we should get the first process arrived which is the top on in the queue
            if(cur_pro==-1)
            {
                // if queue is not empty take the top process start it and change its state
                if(queue_empty(q)==false)
                {
                    cur_pro=queue_front(q);
                    queue_pop(q);
                    pcb[cur_pro]->state=STARTED;
                    kill(pcb[cur_pro]->pid,SIGCONT);
                    //___________print___________
                    printstate(cur_pro);
                }
            }
        break;
        case SJF:
            // no current running process then we should get the process with shortest running time which is the root of the heap 
            if(cur_pro==-1)
            {
                // if heap is not empty take the root process start it and change its state
                if(Min_Heap_empty(heap)==false)
                {
                    cur_pro=Min_Heap_getMin(heap);
                    pcb[cur_pro]->state=STARTED;
                    kill(pcb[cur_pro]->pid,SIGCONT);
                    //___________print___________
                    printstate(cur_pro);
                }
            }
        break;

        case HPF:
            // if heap is not empty take the root process as it's the one with highest priority 
            if(Min_Heap_empty(heap)==false)
            {
                int id=Min_Heap_getMin(heap);
                if(cur_pro==-1)
                {
                    // if no current running process, just run the one we got from the ready list and change its state
                    cur_pro=id;
                    pcb[cur_pro]->state=pcb[cur_pro]->state==ARRIVED?STARTED:RESUMED;
                    kill(pcb[cur_pro]->pid,SIGCONT);
                    //___________print___________
                    printstate(cur_pro);
                }
                else if(pcb[id]->priority < pcb[cur_pro]->priority)
                {
                    // if there a process running but the one we took from the ready list has higher priority than the running one
                    // so stop the running process ,insert it in the heap and run the other process
                    pcb[cur_pro]->state=STOPPED;
                    kill(pcb[cur_pro]->pid,SIGSTOP);
                    Min_Heap_add(heap,cur_pro,pcb[cur_pro]->priority);
                    //___________print___________
                    printstate(cur_pro);

                    kill(pcb[id]->pid,SIGCONT);
                    pcb[id]->state=pcb[id]->state==ARRIVED?STARTED:RESUMED;
                    cur_pro=id;
                    //___________print___________
                    printstate(cur_pro);

                }
                else
                {
                    // otherwise we can't run the process we got from the heap(ready list) so return it back to the heap
                    Min_Heap_add(heap,id,pcb[id]->priority);
                }
            }
        break;
        case SRTN:
          // if heap is not empty take the root process as it's the one with shortest remaining time
            if(Min_Heap_empty(heap)==false)
            {
                int id=Min_Heap_getMin(heap);
                if(cur_pro==-1)
                {
                    // if no current running process, just run the one we got from the ready list and change its state
                    cur_pro=id;
                    pcb[cur_pro]->state=pcb[cur_pro]->state==ARRIVED?STARTED:RESUMED;
                    kill(pcb[cur_pro]->pid,SIGCONT);
                    //___________print___________
                    printstate(cur_pro);

                }
                else if(pcb[id]->remaining_time < pcb[cur_pro]->remaining_time)
                {
                    // if there a process running but the one we took from the ready list has higher priority than the running one
                    // so stop the running process ,insert it in the heap and run the other process
                    pcb[cur_pro]->state=STOPPED;
                    kill(pcb[cur_pro]->pid,SIGSTOP);
                    Min_Heap_add(heap,cur_pro,pcb[cur_pro]->remaining_time);
                    //___________print___________
                    printstate(cur_pro);

                    kill(pcb[id]->pid,SIGCONT);
                    pcb[id]->state=pcb[id]->state==ARRIVED?STARTED:RESUMED;
                    cur_pro=id;
                    //___________print___________
                    printstate(cur_pro);

                }
                else
                {
                    // otherwise we can't run the process we got from the heap(ready list) so return it back to the heap
                    Min_Heap_add(heap,id,pcb[id]->remaining_time);
                }
            }            
        break;
        case RR:
            // we can only run a new process if there is no current running process or the current running process has finished its quantum
            if(queue_empty(q)==false && (cur_pro==-1 || cur_pro_quantum==0))
            {
                if(cur_pro!=-1)
                {
                    // if there a process running but it has finished its quantum time
                    // so stop the running process ,insert it in the queue
                    pcb[cur_pro]->state=STOPPED;
                    kill(pcb[cur_pro]->pid,SIGSTOP);
                    queue_push(q,cur_pro);
                    //___________print___________
                    printstate(cur_pro);
                }
                // if we reach this part of the code this means either no running process or its quantum has finished
                // so we ought to get the front process from the queue
                cur_pro=queue_front(q);
                queue_pop(q);
                kill(pcb[cur_pro]->pid,SIGCONT);
                pcb[cur_pro]->state=pcb[cur_pro]->state==ARRIVED?STARTED:RESUMED;
                //___________print___________
                printstate(cur_pro);
            }
        break;
    }

}


// this integer holds the last value of the clock which helps us to keep track of the change of the clock
int prevtime=0;

// this function returns boolean represents whether the time has changed or not and updates the prevtime variable
// it also changes the running processs remaining time and updates the current running process remaining quantum
bool timestep()
{
    if(getClk()==prevtime)
        return false;
    prevtime=getClk();
    if(cur_pro!=-1)
        pcb[cur_pro]->remaining_time--;
    if(cur_pro_quantum>0)    
        cur_pro_quantum--;
    return true;    
}


// this function is responsible for printing some process into the output file 
void printstate(int id)
{
    int waiting_time=(getClk()-pcb[id]->arrival_time)-(pcb[id]->running_time-pcb[id]->remaining_time);
    char* hash[]={"arrived","started","stopped","resumed","finished"};
    fprintf(log_out,"#At\ttime\t%d\tprocess\t%d\t%s\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n",
    getClk(),id,hash[pcb[id]->state],pcb[id]->arrival_time,pcb[id]->running_time,pcb[id]->remaining_time,waiting_time);
    fflush(log_out);
}

// free everything
void clearResources(int signum)
{
  //TODO Clears all resources in case of interruption
  printf("\nscheduler: terminating...\n");
  destroyClk(true);
  exit(0);
}