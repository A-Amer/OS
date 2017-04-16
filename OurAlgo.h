
#ifndef OURALGO_H
#define OURALGO_H
#include "Scheduler.h"

class OurAlgo:public Scheduler {
    bool operator<(const struct RunProcess& a, const struct RunProcess& b) {
        return a.priority<b.priority && a.ArrTime-b.ArrTime<=2;
    }
public:
    OurAlgo(int ContextSwitch);
    void InsertNewReady(Proc * process);
    void InsertReady(Proc* process,int CurrTime);
    CPUs Schedule(short CpuNo); 
private:
    priority_queue<RunProc> IoBound;//for io bound processes
    priority_queue<RunProc> CpuBound;//for cpu bound processes
    static char IoQuant=2;
    static char CpuQuant=4;
     
};

#endif /* OURALGO_H */

