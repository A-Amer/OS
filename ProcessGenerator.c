#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include<sys/shm.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/shm.h>
#define STDIN_FILENO   0 
#define STDOUT_FILENO  1
#include"Defines.h"
#include"CPU.h"
#include"MessageQueue.h"


void Terminate();
Proc * tail=NULL;
int PID[3];
int* counter; //timer

int* flag;//used to tell how many process arrived currently
void *shmaddr;
int main(int argc, char** argv)
{


int shmid;

//key_t key=12345;


//////////////////////////////////////////////////////////////////////////////////

FILE *p_file;
int Number[5],i,ProcessNumber=0,j;
char c;
Proc p;
printf("I am process gen");
signal(SIGINT,Terminate);
shmid=shmget(FlagKey,sizeof(int), IPC_CREAT|0644);
flag=(int*)shmat(shmid, (void *)0, 0);

shmid=shmget(CounterKey,sizeof(int),IPC_CREAT|0644);
counter=(int*)shmat(shmid, (void *)0, 0);


p_file =fopen("configuration_file.txt","r"); //open configurration in Read Mode
i=0;
c=fgetc(p_file);
while(c!=EOF)//reading fom each file its parameters
    {
     while(c=='#'||c=='\r'||c=='\n'||c==' ')  //ignoring comments line
            {
                    c=fgetc(p_file);

                    while(c!='\n')

                    c=fgetc(p_file);


                    c=fgetc(p_file);

              }
     
     Number[i]=atoi(&c);
     c=fgetc(p_file);
     while(c>47&& c<58){
         Number[i]=Number[i]*10+atoi(&c);
         c=fgetc(p_file);
     }
    i++;
    
}

*counter=0;
 *flag=0;
PID[1]=fork();
    if(PID[1]==0){//start with Process Generator
        char* argv[2]={"scheduler_dispatcher",(char*)0};
        int x=execv("../Scheduler&Dispatcher/dist/Debug/GNU-Linux/scheduler_dispatcher",argv);
    }
    PID[2]=fork();
    if(PID[2]==0){//start with Process Generator
        char Pcount[5];
        char* argv[2]={"devices",(char*)0};
        int x=execv("../Devices/dist/Debug/GNU-Linux/devices",argv);
    }
struct Process  *ProcessArr= malloc( Number[0]*sizeof(Proc));
shmid = shmget(ProcArrKey, Number[0]*sizeof(Proc), IPC_CREAT|0644);
int init=msgget(QKey,IPC_CREAT|0666);
Config con;
for(i=0;i<5;i++){
    con.Numbers[i]=Number[i];
}
con.mtype=3;
msgsnd(init,&con,sizeof(Config),!IPC_NOWAIT);
  
if(shmid == -1)
  	{
  	  perror("Error in create");
  	  exit(-1);
  	}
  
shmaddr = shmat(shmid, (void *)0, 0);//attaching memory
 Proc *address;
 
address=(Proc *)shmaddr;
fclose(p_file);


char s[14]="process_x.txt";
char  n[5];
int d;
////////////////////////////////////////////////////////////////////////////////////
for(i=1;i<=Number[0];i++)
{
    s[8]=i+'0';
    
    p_file =fopen(s,"r");
    c=fgetc(p_file);

    j=0;
    p.Pnum=i;

    while(c!=EOF)//reading fom each file its parameters
    {
            while(c=='#')  //ignoring comments line
            {
                    c=fgetc(p_file);

                    while(c!='\n')

                    c=fgetc(p_file);


                    c=fgetc(p_file);

              }

             if(c!=EOF)
                {

                    if(c=='s') //size
                            {


                            fscanf(p_file,"%s %d",n, &d);
                            p.size=d;

                            }

                    else if(c=='p' || c=='I'|| c=='C' || c=='c')  //PRINTER || I/O || CD || cpu
                            {

                            fscanf(p_file,"%s %d",n, &d);
                            p.arr[j].time=d;
                            p.arr[j].type=c;
                            j++;


                            }


                    else if(c=='a') //arrivaltime

                            {

                            fscanf(p_file,"%s %d",n, &d);
                            p.arrivaltime=d;

                            }
                    else if(c=='P')
			{
                            fscanf(p_file,"%s %d",n, &d);
                            p.priroity=d;
			}

                }
                        c=fgetc(p_file);
    }

    fclose(p_file);

    p.index=0;
    p.pid=-1;
    p.arr[j].time=0;
    p.arr[j].type=0;
    ProcessArr[ProcessNumber]=p;

    ProcessNumber++;

//used for checking process 

/*printf("size=%d arrivaltime=%d index=%d pid=%d \n  "    ,p.size,p.arrivaltime,p.index,p.pid);
int x;
for(x=0;x<j;x++)
{
printf(" type= %c  time=%d",p.arr[x].type,p.arr[x].time);



}*/

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

j=0; //this is to check process turn to be arrived

//wakes up to check arrival
while(j<Number[0]){
    int count=0;
    for(i=0;i<Number[0] && *counter==ProcessArr[j].arrivaltime;i++)

    {		     
      count++;
      address[j]=ProcessArr[j];
      j++;
    }
    if(count==0)
      *flag=-1;
    else
      *flag=count;

    tail=address+j-1;
    
   sleep(8);
   *counter+=1;
   printf("\n count %d",*counter);
    kill(PID[1],SIGCONT);
    kill(PID[2],SIGCONT);
}
while(1){
   sleep(8);
   *flag=-1;
   *counter+=1;
   printf("\n count %d",*counter);

   kill(PID[1],SIGCONT);

   kill(PID[2],SIGCONT);
}

}
void Terminate(){
    int shmid=shmget(FlagKey,sizeof(int), IPC_CREAT|0644);
    shmctl(shmid, IPC_RMID,0);
    shmdt(shmaddr);
    shmdt(counter);
    shmid=shmget(CounterKey,sizeof(int), IPC_CREAT|0644);
    shmctl(shmid, IPC_RMID, 0);	
    kill(PID[2],SIGKILL);
    raise(SIGKILL);
}