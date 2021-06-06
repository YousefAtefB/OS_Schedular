#include "headers.h"

#include "string.h"
#include "Queue.c"

// algorithms
#define FCFS 1

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

// queue to hold the id of the arrived processes in case of alogrithm type fcfs
queue *q;

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
    int waiting_time;  
    int state;
};

typedef struct pcb_node pcb_node;

// the pcb
pcb_node **pcb;

// struct used to recieve element from the process generator
struct process_arrived
{
    int id;
    int arrival_time;
    int running_time;
    int priority;
};

typedef struct process_arrived process_arrived;


// OUTPUT NOTE: whenever you encounter ___________print___________  means we should output to file here


void initialize();
void arrived();
void schedule();

int main(int argc, char *argv[])
{
    initClk();

    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.

    initialize();

    while(true)
    {
        arrived();
        schedule();
        timestep();
    }

    destroyClk(true);
}


//this function is responsible for initializing the scheduler, recieving the  algorithm type and the number of proccesses
void initialize()
{
    // recieving the algorithm type
    algo_typ=FCFS;
    
    // intialize the chosen algo
    switch(algo_typ)
    {
        case FCFS:
            // intializing a queue used in the algo 
            q=queue_init();
        break;
    }

    // recieving the number of processes
    int N=2;
    pcb=malloc(sizeof(pcb_node*)*(N+1));

    for(int i=0;i<=N;i++)
        pcb[i]=NULL;
}

// this function is responsible for recieving processes from the proccess_generator,fork it, store it in the pcb and put its id in the 
// datastructure of the algorithm
void arrived()
{
    // recieving process
    process_arrived temp;

    // making a new block for the process
    pcb[temp.id]=malloc(sizeof(pcb_node));

    // inserting the process into the pcb
    pcb[temp.id]->arrival_time=temp.arrival_time;
    pcb[temp.id]->running_time=temp.running_time;
    pcb[temp.id]->priority=temp.priority;
    pcb[temp.id]->remaining_time=temp.running_time;
    pcb[temp.id]->waiting_time=0;
    pcb[temp.id]->state=ARRIVED;

    int pid=fork();
    if(pid==0)
    {
        // get current working directory
        char cwd[200];
        getcwd(cwd,200);
        strcat(cwd,"/process.out");
        // turn running time into string
        char str_running_time[10];
        sprintf(str_running_time,"%d",temp.running_time);
        // swap the child with the process code and send it the running time
        execl(cwd,str_running_time,NULL);
    }

    pcb[temp.id]->pid=pid;

    // stop the signal after forking it so it won't start right away
    kill(pid,SIGSTOP);

    // insert the process id inside the algorithm datastructure
    switch (algo_typ)
    {
        case FCFS:
            queue_push(q,temp.id);
        break;

    }

}



// this function is responsible for scheduling the processes based on the chosen algo
void schedule()
{

    switch (algo_typ)
    {
        case FCFS:
            //check if current running process finished remove it and remove its ((block)) change its state
            if(cur_pro!=-1)
            {
                if(pcb[cur_pro]->remaining_time<=0)
                {    
                    cur_pro=-1;
                    pcb[cur_pro]->state=FINISHED;
                    //___________print___________
                    free(pcb[cur_pro]);
                    pcb[cur_pro]=NULL;
                }
            }
            // no current running process then we should get the first process arrived which is the top on in the queue
            if(cur_pro==-1)
            {
                // if queue is not empty take the top process start it and change its state
                if(queue_empty()==0)
                {
                    cur_pro=queue_front(q);
                    queue_pop(q);
                    pcb[cur_pro]->state=STARTED;
                    kill(pcb[cur_pro]->pid,SIGCONT);
                }
            }
        break;

    }

}


// this integer holds the last value of the clock which helps us to keep track of the change of the clock
int prevtime=0;

// this function returns boolean represents whether the time has changed or not and updates the prevtime variable
// it also changes the running processs remaining time
bool timestep()
{
    if(getClk()==prevtime)
        return false;
    prevtime=getClk();
    if(cur_pro!=-1)
        pcb[cur_pro]->remaining_time--;
    return true;    
}