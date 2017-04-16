
#ifndef SCHEDULER_H
#define SCHEDULER_H
#include"CPU.h"




typedef struct RunProcess{
    bool IsCpu;
    float priority;
    short CpuTime;//cpu time currently
    short ArrTime;
    Proc * process;
    

}RunProc;
class Scheduler{
public:
   virtual void InsertNewReady(Proc * process){}
   virtual void InsertReady(Proc* process,int CurrTime){}
   virtual CPUs Schedule(short CpuNo){} //CpuNo will be used by our algorithm(cpu1 io,cpu2 cpu)
    
};



#endif /* SCHEDULER_H */
