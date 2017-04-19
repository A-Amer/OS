
#ifndef DISPATCHER_H
#define DISPATCHER_H
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <unistd.h>
#include<cstdlib>
#include<iostream>
#include"CPU.h"
#include"MessageQueue.h"
#include"LinkedList.h"
#include"Scheduler.h"

using namespace std;

enum{Io,CD,Printer};

class Dispatcher {
public:
    
    Dispatcher(int CST,int Memory,int MaxPNo, Scheduler *S);
    void RemoveProcess(Process * P, int CpuNo);
    void ReturnFromBlocked();
    void CheckSwitch(Process * P, int CpuNo);
    void EndOfSeq(Process * P, int CpuNo);
    void RunProcess(int CpuNo);
    void PrepareProcess(int CpuNo);
    void SendProcToIO(int CpuNo,SendInteger FIO);
    void InsertNew(CPU PNew,Process * POnCpu,int CpuNo);
    void Dispatch();
    virtual ~Dispatcher();

private:
    CPU Cpu[2];
    Scheduler *Schd;
    
    CPU Switch[2];
    Process * SendToIO[2];
    LinkedList BlockedForMemory;
    
    int MemoryAvailable;
    int ProcessCounter;
    int KilledProcNo;
    int TotProcessesNo;
    int ContextSwitchingTime;
    
    int * flaginserted;
    int ToIOQueue;
    int FromIOQueue;
    int ToProcQueue;
};

#endif /* DISPATCHER_H */