
#ifndef OURALGO_H
#define OURALGO_H
#include "Scheduler.h"
#include <queue>
#include <vector>

using namespace std;
class RunProc{
public:
    bool IsCpu;
    float priority;
    short CpuTime;//cpu time currently
    short ArrTime;
    Proc * process;
    friend bool operator<(const  RunProc& a, const  RunProc& b) {
        return a.priority<b.priority && a.ArrTime-b.ArrTime<=2;
 }
};

 
class OurAlgo:public Scheduler {
   
public:
    OurAlgo(int ContextSwitch){
        IoQuant=2;
        CpuQuant=4;
        index=-1;
        if(IoQuant<ContextSwitch){
            IoQuant=ContextSwitch+1;
            CpuQuant=5;
        } 
    }
    void InsertNewReady(CPU c1,CPU c2, LinkedList BlockedList, int MemoryAvailable,int IOReturnNo);
    void InsertReady(Proc* process);
    CPUs Schedule(short CpuNo); 
private:
    priority_queue<RunProc> IoBound;//for io bound processes
    priority_queue<RunProc> CpuBound;//for cpu bound processes
    char IoQuant;
    char CpuQuant;
   
     
};

#endif /* OURALGO_H */

