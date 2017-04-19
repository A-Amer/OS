#include <fstream>
#include<math.h>
#include "Dispatcher.h"
#include"Defines.h"
#include "RoundRobin.h"
#include "STRN.h"
#include "HPF.h"
#include "OurAlgo.h"

Dispatcher::Dispatcher(int CST,int Memory,int MaxPNo, Scheduler *S)
{
    Dispatcher::ProcessCounter = 0;
    ContextSwitchingTime = CST;
    MemoryAvailable = Memory;
    TotProcessesNo = MaxPNo;
    ToIOQueue = msgget(BlockedQKey,IPC_CREAT|0666);
    FromIOQueue = msgget(ToReadyQKey,IPC_CREAT|0666);
    ToProcQueue = msgget(QKey,IPC_CREAT|0666);
    shmget(SharedQ, MaxPNo*sizeof(KilledQ), IPC_CREAT|0644);
    int shmid=shmget(FlagKey,sizeof(int), IPC_CREAT|0644);
    flaginserted=(int*)shmat(shmid, (void *)0, 0);
    shmid = shmget(SharedQ, MaxPNo*sizeof(KilledQ), IPC_CREAT|0644);
    KilledP = (KilledQ*)shmat(shmid, (void *)0, 0);
    CpuUtaliz[0]=0;
    CpuUtaliz[1]=0;
    shmid=shmget(CounterKey,sizeof(int),IPC_CREAT|0644);
    Counter=(int*)shmat(shmid, (void *)0, 0);
    Schd = S;
}


Dispatcher::~Dispatcher() 
{
    delete Schd;
    kill(getpgrp(),SIGINT);
    int shmid = shmget(SharedQ, TotProcessesNo *sizeof(KilledQ), IPC_CREAT|0644);
    shmctl(shmid, IPC_RMID, (shmid_ds*)0);
    msgctl(ToProcQueue,IPC_RMID,0);
    
}

void Dispatcher::CheckSwitch(Process* P, int CpuNo)
{
    if(Switch[CpuNo].process==P)
    {
        Cpu[CpuNo]=Switch[CpuNo];
        Switch[CpuNo].process=NULL;
        Switch[CpuNo].RemainingCycle=0;
    }
    else
    {
        kill(P->pid,SIGTSTP);
        Cpu[CpuNo].process = NULL;      
        Cpu[CpuNo].RemainingCycle = ContextSwitchingTime;
    }
    
}

void Dispatcher::EndOfSeq(Process* P, int CpuNo)
{
    if((sizeof(P->arr)/sizeof(struct RunningTime))-1==(P->index))
        {
            kill(P->pid,SIGINT);
            MemoryAvailable += P-> size;
            KilledProcNo++;
            ReturnFromBlocked();
        }
        else
        {
            P->index++;
            SendToIO[CpuNo]=P;
        }  
}

void Dispatcher::SendProcToIO(int CpuNo,SendInteger FIO)
{
    msgbuff m;
    m.mtype = Cpu[CpuNo].process-> pid;
    m.mtext[0]=Cpu[CpuNo].process->arr[Cpu[CpuNo].process->index].type;//set**
    int sendflag = msgsnd(ToProcQueue,&m,sizeof(struct msgbuff),!IPC_NOWAIT);
    if(sendflag==-1)
        cout<<"\nsending failed";
    //signal process to continue
    kill(Cpu[CpuNo].process-> pid,SIGUSR1);
    
    IOs send;
    send.mtype = SendToIO[CpuNo]->arr[SendToIO[CpuNo]->index].type;
    send.process = SendToIO[CpuNo];
    int sendcheck = msgsnd(ToIOQueue,&send,sizeof(IOs),!IPC_NOWAIT);
    if(sendcheck==-1)
        cout<<"\nsending failed";
    FIO.flag++;                        
    SendToIO[CpuNo]==NULL;
}

void Dispatcher::RemoveProcess(Process * P,int CpuNo)
{    
    if (P->arr[P->index].time==0)
    {
        EndOfSeq( P, CpuNo);
        PrepareProcess(CpuNo);
        Cpu[CpuNo].process = 0;      
        Cpu[CpuNo].RemainingCycle = ContextSwitchingTime;
    }
    else
    {
        Schd->InsertReady(P);
        PrepareProcess(CpuNo);
        CheckSwitch(Cpu[CpuNo].process,CpuNo);
    }    
}

void Dispatcher::ReturnFromBlocked()
{
    bool removed = true;
    while(removed&&BlockedForMemory.head!=NULL)
    {
        removed=false;
        if(BlockedForMemory.head->p->size <= MemoryAvailable)
        {
            Schd->InsertReady(BlockedForMemory.head->p);
            BlockedForMemory.DeleteHead();
            removed=true;
        }
        
    }
    if(BlockedForMemory.head!=NULL)
    {
        Node * p1 = BlockedForMemory.head;
        Node * p2 = BlockedForMemory.head->next;
        while(p2!=NULL)
        {
            if(p2->p->size <= MemoryAvailable)
            {
                Schd->InsertReady(p2->p);
                BlockedForMemory.Delete(p1,p2);
            }
            else
            {
                p1=p1->next;
                p2=p2->next;
            }
        }
    }
}

void Dispatcher::RunProcess(int CpuNo)
{ 
    if(Switch[CpuNo].process!=NULL) 
    {
        Cpu[CpuNo]= Switch[CpuNo];
        Switch[CpuNo].process=NULL;
        Switch[CpuNo].RemainingCycle = 0;
        //check if it is the first run 
        if((Cpu[CpuNo].process-> pid) == -1)
        {
            ProcessCounter++;
            int PID = fork();
            if(PID==0)
            {
                char Pcount[5];
                char Arrival[5];
                char CpuNum[10];
                sprintf(Pcount, "%d", ProcessCounter);
                sprintf(CpuNum, "CPU_%d", CpuNo);
                sprintf(Arrival, "%d", Cpu[CpuNo].process->arrivaltime);
                char* proc[5]={"Out",Pcount,Arrival,CpuNum,0};//check it
                execve("Out",proc,NULL);
            }
            MemoryAvailable -= Cpu[CpuNo].process-> size;            
        }
        else
        {
            //send data
            msgbuff m;
            m.mtype = Cpu[CpuNo].process-> pid;
            m.mtext[0] = 'C';
            m.mtext[1] = 'P';
            m.mtext[2] = 'U';
            m.mtext[3]=CpuNo;
            int sendcheck = msgsnd(ToProcQueue,&m,sizeof(struct msgbuff),!IPC_NOWAIT);
            if(sendcheck==-1)
                cout<<"\nsending failed";
            //signal process to continue
            kill(Cpu[CpuNo].process-> pid,SIGUSR1);
        }    
    }
}

void Dispatcher::PrepareProcess(int CpuNo)
{
    bool done=false;
    while(!done)
    {
	CPU P = Schd->Schedule(CpuNo);

        if(P.process==NULL)
        {
            Switch[CpuNo].process = NULL ;
            Switch[CpuNo].RemainingCycle = 0;
            done = true;
        }  
        else if((P.process-> pid) == -1 && MemoryAvailable < P.process-> size)//check if it is the first run & for memory      
            BlockedForMemory.Insert(P.process);
        
        else
        {
            Switch[CpuNo]= P;
            done = true;
        }
    }
}

void Dispatcher::Dispatch()
{   
    CPUs NewInserted[2];
    SendInteger FIO;
    FIO.mtype = flg;
    SendInteger FIORec;
    FIORec.mtype= flg;
    bool NI ;
    int sizercv;
    while(KilledProcNo!=TotProcessesNo)
    {   
        NI=false;
        //read flag of inserted**
        while(*flaginserted==0);
        sizercv = msgrcv(FromIOQueue,&FIORec,sizeof(SendInteger),FIORec.mtype,!IPC_NOWAIT);
            if(sizercv == -1)
                cout << "\nreceiving failed!";
        if(FIORec.flag>0||*flaginserted>0)//||flag inserted greater than zero
        {
            NI=true;
            for(int j = 0; j < 2; j++)
            {
                if(Cpu[j].process==NULL && Cpu[j].RemainingCycle > 0)
                    NewInserted[j].process = Switch[j].process;
                else
                    NewInserted[j].process = Cpu[j].process;
                if(Cpu[j].process!=NULL)
                    Cpu[j].process->arr[Cpu[j].process->index].time--;
            }      
        }
        for(int i = 0; i < 2; i++)
        {
            if(Cpu[i].process==NULL && Cpu[i].RemainingCycle > 0)//context switching
            {
                Cpu[i].RemainingCycle--;
                if(NewInserted[i].process != NULL)
                {   
                    Switch[i]= NewInserted[i];
                    NewInserted[i].process = NULL;
                    NewInserted[i].RemainingCycle = 0;
                }
                if(Cpu[i].RemainingCycle==0)
                {
                    if(SendToIO[i]!=NULL)//send to IO
                        SendProcToIO(i,FIO);
                    
                    if(Switch[i].process == NULL)
                        PrepareProcess(i);
                    
                    RunProcess(i);
                }
                
            }
            else if(Cpu[i].process!=NULL)//if a process was running on the CPU
            {
                if(!NI)
                    Cpu[i].process->arr[Cpu[i].process->index].time--;
                Cpu[i].RemainingCycle--;
                if(NewInserted[i].process==NULL)
                {
                    if(Cpu[i].RemainingCycle == 0)//if the cycle is over
                        RemoveProcess(Cpu[i].process,i); 
                }
                else
                {
                    InsertNew(NewInserted[i],Cpu[i].process,i);
                    NewInserted[i].process = NULL;
                    NewInserted[i].RemainingCycle = 0; 
                }
                CpuUtaliz[i]++;
            }
            else //if no process was running on the CPU
            {
                if(NewInserted[i].process == NULL)
                    PrepareProcess(i);
                else
                {
                    Switch[i]= NewInserted[i];
                    NewInserted[i].process = NULL;
                    NewInserted[i].RemainingCycle = 0;    
                }
                RunProcess(i);
            }
        }
                
        int sendcheck = msgsnd(ToIOQueue,&FIO,sizeof(SendInteger),!IPC_NOWAIT);
        if(sendcheck==-1)
            cout<<"\nsending failed";
        FIO.flag=0;
        pause();
    }
    ofstream MyFile;
    MyFile.open("CPU_1.txt",ios::app);   
    MyFile<<"\nCPU Utilization = "<<CpuUtaliz[0]/((*Counter)-1);
    MyFile.close();
    
    MyFile.open("CPU_2.txt",ios::app);   
    MyFile<<"\nCPU Utilization = "<<CpuUtaliz[1]/((*Counter)-1);
    MyFile.close();
    
    MyFile.open("Stat.txt",ios::trunc);
    //create the other file to write into **
    int AvgWTA=0,AvgWait=0,Std=0;
    for(int i = 0 ; i < TotProcessesNo ; i++)
    {
        AvgWTA+=KilledP[i].WTA;
        AvgWait+=KilledP[i].Wait;
        
    }
    AvgWTA=AvgWTA/TotProcessesNo;
    AvgWait=AvgWait/TotProcessesNo;
    MyFile<<"\nAverage WTA = "<<AvgWTA;
    MyFile<<"\nAverage Wait = "<<AvgWait<<"\n";
    for(int i = 0 ; i < TotProcessesNo ; i++){
        Std+=(KilledP[i].WTA-AvgWTA)*(KilledP[i].WTA-AvgWTA);
    }
    Std=Std/TotProcessesNo;
    Std=sqrt(Std);
    MyFile<<"\nStandard deviation of WTA = "<<Std<<"\n";
    MyFile.close();
    
}

void Dispatcher::InsertNew(CPU PNew,Process * POnCpu,int CpuNo)
{
    if (POnCpu->arr[POnCpu->index].time==0)
        EndOfSeq(POnCpu, CpuNo);
    else
    {
        Schd->InsertReady(POnCpu);
        kill(POnCpu-> pid,SIGTSTP);  
    }

    Cpu[CpuNo].process = NULL;      
    Cpu[CpuNo].RemainingCycle = ContextSwitchingTime;        
    Switch[CpuNo]=PNew;
}
int main(){
    int init=msgget(QKey,IPC_CREAT|0666);
    Config con;
    msgrcv(init,&con,sizeof(Config),0,!IPC_NOWAIT);
    Scheduler* Sched=NULL;
    switch(con.Numbers[1]){
        case(1):
                Sched= new RoundRobin(con.Numbers[3],con.Numbers[0]);
        case(2):
                Sched= new STRN();
        case(3):
                Sched= new HPF();
        case(4):
                Sched= new OurAlgo(con.Numbers[2]);
                        
    }
    Dispatcher* Disp=new Dispatcher(con.Numbers[2],2000000,con.Numbers[0],Sched);
    Disp->Dispatch();
    delete Disp;
}



