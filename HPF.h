
#ifndef HPF_H
#define HPF_H
#include"LinkedList.h"
#include"Scheduler.h"
class HPF :public Scheduler{
public:
   void InsertNewReady(Proc * p1,Proc * p2);
    void InsertReady(Proc* process,int CurrTime);
    CPUs Schedule(short CpuNo);
    int HPF(Process *p);
private:
    LinkedList List;
};

#endif /* HPF_H */

