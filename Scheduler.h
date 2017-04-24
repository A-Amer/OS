
#ifndef SCHEDULER_H
#define SCHEDULER_H
#include"CPU.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include<sys/msg.h>
#include<fstream>
#include <unistd.h>
#include"Defines.h"
#include"LinkedList.h"
#include"MessageQueue.h"
using namespace std;

 
class Scheduler{
 
protected:
    Proc* ProcessArr;
    int* flag;
    int FromIOQueue;
    int index;
public:
   virtual void InsertNewReady(CPU * c1,CPU * c2, LinkedList * BlockedList, int MemoryAvailable,int IOReturnNo)=0;
   virtual void InsertReady(Proc* process)=0;
   virtual CPUs Schedule(short CpuNo)=0; //CpuNo will be used by our algorithm(cpu1 io,cpu2 cpu)
      Scheduler(){
        int shmid;
        index=-1;
        FromIOQueue = msgget(ToReadyQKey,IPC_CREAT|0666);
        shmid=shmget(FlagKey,sizeof(int), IPC_CREAT|0644);
        flag=(int*)shmat(shmid, (void *)0, 0);
        shmid = shmget(ProcArrKey, sizeof(Proc), IPC_CREAT|0644);
        ProcessArr=(Proc*)shmat(shmid, (void *)0, 0);
    }
    ~Scheduler(){
        int shmid = shmget(ProcArrKey, sizeof(Proc), IPC_CREAT|0644);
        shmctl(shmid, IPC_RMID, (shmid_ds*)0);
    } 
};



#endif /* SCHEDULER_H */
