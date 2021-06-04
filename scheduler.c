#include "headers.h"

#include "string.h"


//this integer indicates the algorithm currently being used
int algo_typ;

// struct represent one element of the pcb
struct pcb_node
{
    int arrival_time;
    int running_time;
    int priority;
    int remaining_time;
    int waiting_time;
    int state;
};

typedef struct pcb_node pcb_node;

// the pcb
pcb_node *pcb;

// struct used to recieve element from the process generator
struct process_arrived
{
    int id;
    int arrival_time;
    int running_time;
    int priority;
};

typedef struct process_arrived process_arrived;

void initialize();
void arrived();

int main(int argc, char *argv[])
{
    initClk();

    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.

    initialize();

    while(true)
    {
        arrived();
    }

    destroyClk(true);
}


//this function is responsible for initializing the scheduler, recieving the  algorithm type and the number of proccesses
void initialize()
{
    // recieving the algorithm type
    algo_typ=1;
    
    // recieving the number of processes
    int N=2;
    pcb=malloc(sizeof(pcb_node)*(N+1));
}

// this function is responsible for recieving processes from the proccess_generator,fork it, store it in the pcb and put its id in the 
// datastructure of the algorithm
void arrived()
{
    // recieving process
    process_arrived temp;

    // inserting the process into the pcb
    pcb[temp.id].arrival_time=temp.arrival_time;
    pcb[temp.id].running_time=temp.running_time;
    pcb[temp.id].priority=temp.priority;
    pcb[temp.id].remaining_time=temp.running_time;
    pcb[temp.id].waiting_time=0;
    pcb[temp.id].state=0;

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


    

}