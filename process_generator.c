#include "headers.h"
#include "generatorQueue.c"
#include <string.h>
void clearResources(int);

struct msgbuff
{
  long mtype;
  struct process *p;
  bool stop; //if true, stop receiving
};

int msgqid;
int main(int argc, char *argv[])
{

  signal(SIGINT, clearResources);
  msgqid = msgget(12613, IPC_CREAT | 0644);
  if (msgqid == -1)
  {
    perror("Error in create");
    exit(-1);
  }
  printf("msgqid = %d\n", msgqid);

  struct queue *q;
  q = malloc(sizeof(struct queue));
  initialize(q);
  // 1. Read the input files.
  FILE *inFile = fopen("processes.txt", "r");
  if (inFile == NULL)
  {
    printf("Error! Could not open file\n");
    exit(-1);
  }
  struct stat sb;
  if (stat("processes.txt", &sb) == -1)
  {
    perror("stat");
    exit(EXIT_FAILURE);
  }
  char *buff = malloc(sb.st_size);

  while (fscanf(inFile, "%s", buff) != EOF)
  {
    if (buff[0] == '#')
    {
      fscanf(inFile, "%[^\n] ", buff);
    }
    else
    {
      struct process *p1 = malloc(sizeof(struct process));
      p1->id = atoi(buff);
      fscanf(inFile, "%s", buff);
      p1->arrival = atoi(buff);
      fscanf(inFile, "%s", buff);
      p1->runtime = atoi(buff);
      fscanf(inFile, "%s", buff);
      p1->priority = atoi(buff);
      enqueue(q, p1);
    }
  }

  fclose(inFile);
  display(q->front);

  // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
  int chosen_algo = atoi(argv[1]);
  int num_parms = argc - 1;
  char **parms;
  parms = malloc((argc + 1) * sizeof(char *)); //all arguments passed to the funcion
  parms[0] = malloc(50 * sizeof(char));
  parms[0] = "./scheduler.out";
  for (int i = 1; i < num_parms + 1; i++)
  {
    parms[i] = malloc(50 * sizeof(char));
    parms[i] = argv[i];
    printf("%s \n", parms[i]);
  }
  parms[num_parms] = malloc(50 * sizeof(char));
  parms[num_parms] = NULL;

  // 3. Initiate and create the scheduler and clock processes.
  int *PID;
  PID = (int *)malloc(2 * sizeof(int));
  for (int i = 0; i < 2; i++)
  {

    PID[i] = fork();

    if (PID[i] == -1)
    {
      printf("Error In Forking\n");
    }

    else if (PID[i] == 0)
    {

      if (i == 1)
      {
        printf("I'm schedular, My process ID is %d, my parent ID : %d \n", getpid(), getppid());
        execv("./scheduler.out", parms);
      }
      else
      {
        printf("I'm clock, My process ID is %d, my parent ID : %d \n", getpid(), getppid());
        execl("./clk.out", "./clk.out", NULL);
      }
    }
  }

  // 4. Use this function after creating the clock process to initialize clock.
  initClk();
  // 5. Create a data structure for processes and provide it with its parameters.

  //done before

  // 6. Send the information to the scheduler at the appropriate time.
  int x = getClk();
  printf("Current Time is %d\n", x);

  while (!(isempty(q)))
  {
    int temp = getClk();
    if (temp != x)
    {
      x = temp;
      printf("Current Time is %d\n", x);
      display(q->front);
    }

    int t = q->front->proc->arrival;

    while ((t <= x) && !(isempty(q)))
    {
      //now starting to send the process to the scheduler
      int send_val;
      struct msgbuff message;
      message.mtype = 7; /* arbitrary value */
      message.stop = false;
      printf("Dequeuing ...\n");
      message.p = dequeue(q);
      send_val = msgsnd(msgqid, &message, sizeof(struct process), !IPC_NOWAIT);
      if (send_val == -1)
      {
        perror("Errror in send");
      }
      display(q->front);
      if (!(isempty(q)))
      {
        t = q->front->proc->arrival;
      }
    }
  }
  int send_val;
  struct msgbuff message;
  message.mtype = 7; /* arbitrary value */
  message.stop = true;
  message.p = NULL;
  send_val = msgsnd(msgqid, &message, sizeof(struct process), !IPC_NOWAIT);
  // 7. Clear clock resources
  destroyClk(true);
  exit(0);
}

void clearResources(int signum)
{
  //TODO Clears all resources in case of interruption
  int send_val;
  struct msgbuff message;
  message.mtype = 7; /* arbitrary value */
  message.stop = true;
  message.p = NULL;
  send_val = msgsnd(msgqid, &message, sizeof(struct process), !IPC_NOWAIT);
  destroyClk(true);
}
