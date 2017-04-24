
#ifndef HPF_H
#define HPF_H
#include"LinkedList.h"
#include"Scheduler.h"
class HPF :public Scheduler{
public:
   void InsertNewReady(CPU * c1,CPU * c2, LinkedList * BlockedList, int MemoryAvailable,int IOReturnNo);
    void InsertReady(Proc* process);
    CPUs Schedule(short CpuNo);
    int HiPF(Process* &p);
private:
    LinkedList List;
};

#endif /* HPF_H */

