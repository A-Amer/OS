#ifndef ROUNDROBIN_H
#define ROUNDROBIN_H
#include"Scheduler.h"


class RoundRobin:public Scheduler {
public:
    RoundRobin(int q,int number);
    void InsertNewReady(Proc * p1,Proc * p2);
    void InsertReady(Proc* process,int CurrTime);
    CPUs Schedule(short CpuNo);
    int  RR(Proc *p); //returns running time

private:
	int quantum;
        int head,tail;
        Proc * * Queue ;
	int n;
};

#endif /* ROUNDROBIN_H */

