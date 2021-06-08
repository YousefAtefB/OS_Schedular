#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char *argv[])
{
    int myid=atoi(argv[1]);
    remainingtime=atoi(argv[2]);

    initClk();

    printf("\nprocess%d at %d: welcome\n",myid,getClk());
    fflush(stdout);


    //TODO The process needs to get the remaining time from somewhere
    //remainingtime = ??;
    int prevtime=getClk();
    while (remainingtime > 0)
    {
        // remainingtime = ??;
        if(prevtime!=getClk())
        {
            prevtime++;
            remainingtime--;
        }
    }

    printf("process%d at %d: goodbye",myid,getClk());

    destroyClk(false);

    return 0;
}
