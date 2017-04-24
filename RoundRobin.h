#ifndef ROUNDROBIN_H
#define ROUNDROBIN_H
#include"Scheduler.h"


class RoundRobin:public Scheduler {
public:
    RoundRobin(int q,int number);
    void InsertNewReady(CPU * c1,CPU * c2, LinkedList * BlockedList, int MemoryAvailable,int IOReturnNo);
    void InsertReady(Proc* process);
    CPUs Schedule(short CpuNo);
    int  RR(Proc *p); //returns running time

private:
	int quantum;
        int head,tail;
        Proc * * Queue ;
	int n;
};

#endif /* ROUNDROBIN_H */

