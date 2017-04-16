
#ifndef OURALGO_H
#define OURALGO_H
#include "Scheduler.h"

class OurAlgo:public Scheduler {
    bool operator<(const struct RunProcess& a, const struct RunProcess& b) {
        return a.priority<b.priority && a.ArrTime-b.ArrTime<=2;
    }
public:
    OurAlgo(int ContextSwitch);
    void InsertReady(Process * process);
    CPU Schedule(); 
private:
    priority_queue<RunProcess> IoBound;//for io bound processes
    priority_queue<RunProcess> CpuBound;//for cpu bound processes
    char IoQuant=2;
    char CpuQuant=4;
     
};

#endif /* OURALGO_H */

