#ifndef STRN_H
#define STRN_H
#include"Scheduler.h"
#include"LinkedList.h"
class STRN :public Scheduler{
public:
    
    void InsertNewReady(CPU * c1,CPU * c2, LinkedList * BlockedList, int MemoryAvailable,int IOReturnNo);
    void InsertReady(Proc* process);
    CPUs Schedule(short CpuNo);
    STRN(){
        l.head=NULL;
        l.tail=NULL;
        l.count=0;
    } 
private:
    LinkedList l;
    Proc * ptail;
};

#endif /* STRN_H */


