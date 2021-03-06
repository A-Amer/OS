
#include "RoundRobin.h"
#include <stdlib.h>
Proc* ptail; // from process generator
RoundRobin::RoundRobin(int q,int number) {
         head=-1;
	tail=0;
	quantum=q;	
	Queue=new Proc*[number];
	n=number;
	for (int  i=0;i<n;i++)
		Queue[i]=NULL;
}
void RoundRobin::InsertNewReady(CPU c1,CPU c2, LinkedList BlockedList, int MemoryAvailable,int IOReturnNo){
             for(int i=1;i<=*flag;i++){
                InsertReady(ProcessArr+index+i);
            }
            index+=*flag;
            *flag=0;
}
void RoundRobin:: InsertReady(Proc* process){
    if(head==-1 && tail==0)
		{
			head++;
			Queue[tail++]=process;
			return ;
		}
		else if(tail==n)
		{
			tail=0;
			Queue[tail++]=process;  //the queue is always applicable to carry total processes so it will be empty at head when tail reaches n
		
		}
		else
		{
			Queue[tail++]=process;
		}
}
int RoundRobin::RR(Proc *p) //returns running time
{
	int index=p->index;
	int Runtime=p->arr[index].time;
	
	if(quantum>Runtime)
	{
          p->arr[index].time=0;
          return  Runtime;
	}
        else
        {
   	     p->arr[index].time=  p->arr[index].time-quantum;
	  return quantum;
	
        }
 	
}
CPUs RoundRobin::Schedule(short CpuNo){
    CPUs c;
    if(head==-1)
            {
                    c.process=NULL;
                    c.RemainingCycle=0;
                    return c;
            }
    else if (head==n)
            {

            head=0;
            if(Queue[head]==NULL)
                    {

                            c.process=NULL;
                            c.RemainingCycle=0;
                            return c;
                    }
                            c.RemainingCycle=RR(Queue[head]);
                            c.process=Queue[head++];
            Queue[head-1]=NULL;
                    return c;

            }
    else
    {
            if(Queue[head]==NULL)
                    {
                            c.process=NULL;
                            c.RemainingCycle=0;
                            return c;
                    }
            Process * temp=Queue[head];
            Queue[head]=NULL;
            head++;
            c.RemainingCycle=RR(temp);

        c.process=temp;

                    return c;

    }
}



