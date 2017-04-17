#include<cstdlib>
#include"OurAlgo.h"

void OurAlgo::InsertNewReady(){
    for(int i=1;i<=flag;i++){
        InsertNewReady(ProcessArr+index+i);
    }
    index+=flag;
    flag=0;//will be changed when flag is shared
}

 void OurAlgo::InsertNewReady(Proc * process){
    int totalTime=0;
    RunProc* NewProcess=new RunProc;
    short io=0;
    for(int i=0;i<20;i++){
        totalTime+=process->arr[i].time;
        if(i%2!=0)
            io+=process->arr[i].time;
    }
    
    NewProcess->ArrTime=process->arrivaltime;
    NewProcess->priority=process->priroity/totalTime;
    NewProcess->process=process;
    NewProcess->CpuTime=process->arr[0].time;
    if(io<totalTime-io)
        NewProcess->IsCpu=true;
    else
        NewProcess->IsCpu=false;
    if(NewProcess->IsCpu)
        CpuBound.push(*NewProcess);
    else
        IoBound.push(*NewProcess);
    
}
void OurAlgo:: InsertReady(Proc* process,int CurrTime){//we could find a way to save priority and iscpu
    int totalTime=0;
    RunProc* NewProcess=new RunProc;
    short io=0;
    for(int i=0;i<20;i++){
        totalTime+=process->arr[i].time;
        if(i%2!=0)
            io+=process->arr[i].time;
    }
    
    NewProcess->ArrTime=CurrTime;
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

