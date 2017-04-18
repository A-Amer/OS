
#ifndef STRN_H
#define STRN_H
#include"Scheduler.h"
#include"LinkedList.h"
class STRN :public Scheduler{
public:
    
    void InsertNewReady(Proc * p1,Proc * p2);
    void InsertReady(Proc* process,int CurrTime=0);
    CPUs Schedule(short CpuNo);
private:
    LinkedList l;
};

#endif /* STRN_H */

