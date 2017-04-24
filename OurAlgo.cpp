#include<cstdlib>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include"OurAlgo.h"
#include "MessageQueue.h"


void OurAlgo::InsertNewReady(CPU * c1,CPU * c2, LinkedList * BlockedList, int MemoryAvailable,int IOReturnNo){
    for(int i=1;i<=*flag;i++){
        InsertReady(ProcessArr+index+i);
    }
    for(int i=0;i<IOReturnNo;i++){
        IOs Recv;
        msgrcv(FromIOQueue,&Recv,sizeof(IOs),0,!IPC_NOWAIT);
        int j=0;
        while((ProcessArr+j)-> pid != Recv.process)
            j++;
        ProcessArr[j].index++;
        InsertReady(ProcessArr+j);
    }
    index+=*flag;
    *flag=0;//will be changed when flag is shared
    c1->process=NULL;
    c1->RemainingCycle=0;
    c2->process=NULL;
    c2->RemainingCycle=0;
}

 
void OurAlgo:: InsertReady(Proc* process){//we could find a way to save priority and iscpu
    int totalTime=0;
    RunProc NewProcess;
    short io=0;
    short i=0;
    while(process->arr[i].type!=0){
        totalTime+=process->arr[i].time;
        if(i%2!=0)
            io+=process->arr[i].time;
        i++;
    }
    int shmid=shmget(CounterKey,sizeof(int),IPC_CREAT|0644);
    int* shmadd=(int*)shmat(shmid, (void *)0, 0);//attaching memory
    
    NewProcess.ArrTime=*shmadd;
    shmdt(shmadd);
    NewProcess.priority=process->priroity/(float)totalTime;
    NewProcess.process=process;
    NewProcess.CpuTime=process->arr[process->index].time;
    if(io<totalTime-io)
        NewProcess.IsCpu=true;
    else
        NewProcess.IsCpu=false;
    if(NewProcess.IsCpu)
        CpuBound.push(NewProcess);
    else
        IoBound.push(NewProcess);

}
CPUs OurAlgo::Schedule(short CpuNo){
    CPUs Process;
    Process.RemainingCycle=0;
    Process.process=NULL;
     if(IoBound.empty()&& CpuBound.empty())
         return  Process;
     RunProc Scheduled;
     
    if(CpuNo==1){
        if(!IoBound.empty()){
            Scheduled=IoBound.top();
            if(Scheduled.CpuTime-IoQuant<=1)
                Process.RemainingCycle=Scheduled.CpuTime;
            else{
                Process.RemainingCycle=IoQuant;
            }
            Process.process=Scheduled.process;
            IoBound.pop();
        }
        else{
             Scheduled=CpuBound.top();
             if(Scheduled.CpuTime-IoQuant<=1)
                Process.RemainingCycle=Scheduled.CpuTime;
            else{
                Process.RemainingCycle=IoQuant;
            }
            Process.process=Scheduled.process;
            CpuBound.pop();
        }
    }
     
     
    else{
        if(!CpuBound.empty()){
            Scheduled=CpuBound.top();
            if(Scheduled.CpuTime-CpuQuant<=1)
                Process.RemainingCycle=Scheduled.CpuTime;
            else{
                Process.RemainingCycle=CpuQuant;
            }
            Process.process=Scheduled.process;
            CpuBound.pop();
        }
        else{
             Scheduled=IoBound.top();
             if(Scheduled.CpuTime-CpuQuant<=1)
                Process.RemainingCycle=Scheduled.CpuTime;
            else{
                Process.RemainingCycle=CpuQuant;
            }
            Process.process=Scheduled.process;
            IoBound.pop();
        }
    }
    return  Process; 
}

