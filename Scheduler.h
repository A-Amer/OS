
#ifndef SCHEDULER_H
#define SCHEDULER_H
#include"CPU.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>


typedef struct RunProcess{
    bool IsCpu;
    float priority;
    short CpuTime;//cpu time currently
    short ArrTime;
    Proc * process;
    

}RunProc;
class Scheduler{
 
protected:
    Proc* ProcessArr;
    int flag;//will be changed when flag is shared
public:
   virtual void InsertNewReady(Proc * p1,Proc * p2)=0;
   virtual void InsertReady(Proc* process,int CurrTime)=0;
   virtual CPUs Schedule(short CpuNo)=0; //CpuNo will be used by our algorithm(cpu1 io,cpu2 cpu)
      Scheduler(){
        int shmid;
        shmid = shmget(12345, 4096, IPC_CREAT|0644);
        ProcessArr=(Proc*)shmat(shmid, (void *)0, 0);
    }
    ~Scheduler(){
        shmdt(ProcessArr);
    } 
};



#endif /* SCHEDULER_H */
