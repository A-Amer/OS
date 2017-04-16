#include "OurAlgo.h"

OurAlgo::OurAlgo(int ContextSwitch){
        if(IoQuant<ContextSwitch){
            IoQuant=ContextSwitch+1;
            CpuQuant=5;
        } 
    }
OurAlgo::InsertReady(Process * process){
    int totalTime=0;
    RunProcess NewProcess=new RunProcess;
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
    if(NewProcess.IsCpu)
        CpuBound.push(NewProcess);
    else
        IoBound.push(NewProcess);
    
}
OurAlgo::Schedule(){
    
}

