#include<cstdlib>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include"OurAlgo.h"


void OurAlgo::InsertNewReady(CPU c1,CPU c2, LinkedList BlockedList, int MemoryAvailable,int IOReturnNo){
    for(int i=1;i<=*flag;i++){
        InsertReady(ProcessArr+index+i);
    }
    index+=*flag;
    *flag=0;//will be changed when flag is shared
}

 
void OurAlgo:: InsertReady(Proc* process){//we could find a way to save priority and iscpu
    int totalTime=0;
    RunProc* NewProcess=new RunProc;
    short io=0;
    for(int i=0;i<20;i++){
        totalTime+=process->arr[i].time;
        if(i%2!=0)
            io+=process->arr[i].time;
    }
    int shmid=shmget(CounterKey,sizeof(int),IPC_CREAT|0644);
    int* shmadd=(int*)shmat(shmid, (void *)0, 0);//attaching memory
    
    NewProcess->ArrTime=*shmadd;
    shmdt(shmadd);
    NewProcess->priority=process->priroity/totalTime;
    NewProcess->process=process;
    NewProcess->CpuTime=process->arr[process->index].time;
    if(io<totalTime-io)
        NewProcess->IsCpu=true;
    else
        NewProcess->IsCpu=false;
    if(NewProcess->IsCpu)
        CpuBound.push(*NewProcess);
    else
        IoBound.push(*NewProcess);
    shmdt(shmadd);
}
CPUs OurAlgo::Schedule(short CpuNo){
    CPUs Process;
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
}

