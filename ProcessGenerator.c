#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/file.h>
#include <sys/shm.h>
#define STDIN_FILENO   0 
#define STDOUT_FILENO  1
#include"CPU.h"

Proc * tail=NULL;

int counter; //timer

int flag0;//used to tell how many process arrived currently

void main()
{


int shmid;
void *shmaddr;
key_t key=12345;


//////////////////////////////////////////////////////////////////////////////////

FILE *p_file;
int Number,i,ProcessNumber=0,j;
char c;
struct Process p;
p_file =fopen("configuration file.txt","r"); //open configurration in Read Mode
 
fscanf(p_file, "%d", &Number);
struct Process  *ProcessArr= malloc( Number*sizeof(struct Process));
shmid = shmget(key, Number*sizeof(Proc), IPC_CREAT|0644);
  
if(shmid == -1)
  	{
  	  perror("Error in create");
  	  exit(-1);
  	}
  
shmaddr = shmat(shmid, (void *)0, 0);//attaching memory
 Proc *address;
address=(Proc *)shmaddr;
fclose(p_file);
char  s[14]="process_x.txt";
char  n[5];
int d;
////////////////////////////////////////////////////////////////////////////////////
for(i=1;i<=Number;i++)
{
s[8]=i+'0';


p_file =fopen(s,"r");
c=fgetc(p_file);

 j=0;


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

                }
                        c=fgetc(p_file);
    }

    fclose(p_file);

    p.index=0;
    p.pid=-1;

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
int count=0;
                     for(i=0;i<Number && count==ProcessArr[j].arrivaltime;i++)
                       
			{
			     
              count++;
	              address[j]=ProcessArr[j];
			j++;


			}
			if(count==0)
				flag0=-1;
			else
               flag0=count;
           
           tail=address+j-1;

}
