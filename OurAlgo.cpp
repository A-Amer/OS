#include"OurAlgo.h"


OurAlgo::OurAlgo(int ContextSwitch){
        if(IoQuant<ContextSwitch){
            IoQuant=ContextSwitch+1;
            CpuQuant=5;
        } 
    }
 OurAlgo::InsertNewReady(Proc * process){
    int totalTime=0;
    RunProc NewProcess=new RunProc;
    short io=0;
    for(int i=0;i<20;i++){
        totalTime+=process->arr[i];
        if(i%2!=0)
            io+=process->arr[i];
    }
    
    NewProcess.ArrTime=process->arrivaltime;
    NewProcess.priority=process->priroity/totalTime;
    NewProcess.process=process;
    NewProcess.CpuTime=process->arr[0];
    if(io<totalTime-io)
        NewProcess.IsCpu=true;
    else
        NewProcess.IsCpu=false;
    if(NewProcess.IsCpu)
        CpuBound.push(NewProcess);
    else
        IoBound.push(NewProcess);
    
}
OurAlgo:: InsertReady(Proc* process,int CurrTime){//we could find a way to save priority and iscpu
    int totalTime=0;
    RunProc NewProcess=new RunProc;
    short io=0;
    for(int i=0;i<20;i++){
        totalTime+=process->arr[i];
        if(i%2!=0)
            io+=process->arr[i];
    }
    
    NewProcess.ArrTime=CurrTime;
    NewProcess.priority=process->priroity/totalTime;
    NewProcess.process=process;
    NewProcess.CpuTime=process->arr[process->index];
    if(io<totalTime-io)
        NewProcess.IsCpu=true;
    else
        NewProcess.IsCpu=false;
    if(NewProcess.IsCpu)
        CpuBound.push(NewProcess);
    else
        IoBound.push(NewProcess);
}
 OurAlgo::Schedule(short CpuNo){
     if(IoBound.empty()&& CpuBound.empty())
         return 0;
     RunProc Scheduled;
     CPUs Process;
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

