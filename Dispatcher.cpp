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
    ContextSwitchingTime = CST;
    MemoryAvailable = Memory;
    TotProcessesNo = MaxPNo;
    ToIOQueue = msgget(BlockedQKey,IPC_CREAT|0666);
    FromIOQueue = msgget(ToReadyQKey,IPC_CREAT|0666);
    ToProcQueue = msgget(QKey,IPC_CREAT|0666);
    int shmid = shmget(SharedQ, MaxPNo*sizeof(KilledQ), IPC_CREAT|0644);
    KilledP = (KilledQ*)shmat(shmid, (void *)0, 0);
    shmid=shmget(FlagKey,sizeof(int), IPC_CREAT|0644);
    flaginserted=(int*)shmat(shmid, (void *)0, 0);
    
    CpuUtaliz[0]=0;
    CpuUtaliz[1]=0;
    shmid=shmget(CounterKey,sizeof(int),IPC_CREAT|0644);
    Counter=(int*)shmat(shmid, (void *)0, 0);
    Schd = S;
}


Dispatcher::~Dispatcher() 
{
    delete Schd;
    shmdt(flaginserted);
    int shmid = shmget(SharedQ, TotProcessesNo *sizeof(KilledQ), IPC_CREAT|0644);
    shmctl(shmid, IPC_RMID, (shmid_ds*)0);
    msgctl(ToProcQueue,IPC_RMID,0);
    int id=msgget(BlockedQKey,IPC_CREAT|0646);
    msgctl(id,IPC_RMID,0);
    id=msgget(ToReadyQKey,IPC_CREAT|0646);
    msgctl(id,IPC_RMID,0);
   
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
    if(P->arr[P->index+1].type==0)
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
            kill(P->pid,SIGTSTP);
            
        }  
}

void Dispatcher::SendProcToIO(int CpuNo,IOs FIO)
{
    IOs send;
    send.mtype = SendToIO[CpuNo]->arr[SendToIO[CpuNo]->index].type;
    send.process = SendToIO[CpuNo]->pid;
    send.RemainingCycle=SendToIO[CpuNo]->arr[SendToIO[CpuNo]->index].time;

    int sendcheck = msgsnd(ToIOQueue,&send,sizeof(IOs),!IPC_NOWAIT);
    if(sendcheck==-1)
        cout<<"\nsending to io failed";
    msgbuff m;
    m.mtype = SendToIO[CpuNo]-> pid;
    m.mtext[0]=SendToIO[CpuNo]->arr[SendToIO[CpuNo]->index].type;
    m.mtext[1]='\0';
    printf("\n sending to pid %d", (int)m.mtype);
    
    int sendflag = msgsnd(ToProcQueue,&m,sizeof(m.mtext),!IPC_NOWAIT);
    if(sendflag==-1)
        cout<<"\nsending failed";
    //signal process to continue
                      
    SendToIO[CpuNo]=NULL;
}

void Dispatcher::RemoveProcess(Process * P,int CpuNo)
{    
    if (P->arr[P->index].time<=0)
    {
        EndOfSeq( P, CpuNo);
        PrepareProcess(CpuNo);
        Cpu[CpuNo].process = 0;      
        Cpu[CpuNo].RemainingCycle = ContextSwitchingTime;
    }
    else
    {
        cout<<"\nreturned in ready";
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
            
            int PID = fork();
            if(PID==0)
            {
                char Pcount[5];
                char Arrival[5];
                char CpuNum[10];
                 sprintf(Pcount, "%d",Cpu[CpuNo].process->Pnum);
                CpuNum[0] = 'c';
                CpuNum[1] = 'p';
                CpuNum[2] = 'u';
                CpuNum[3] = '_';
                CpuNum[4]=CpuNo+1+'0';
                CpuNum[5]='\0';
                sprintf(Arrival, "%d", Cpu[CpuNo].process->arrivaltime);
                char* proc[5]={"output",Pcount,Arrival,CpuNum,0};//check it
                int x=execv("../Output/dist/Debug/GNU-Linux/output",proc);
                printf("\n%d",x);
            }
            Cpu[CpuNo].process-> pid=PID;
            MemoryAvailable -= Cpu[CpuNo].process-> size;            
        }
        else
        {
            //send data
            msgbuff m;
            m.mtype = Cpu[CpuNo].process-> pid;
            m.mtext[0] = 'c';
            m.mtext[1] = 'p';
            m.mtext[2] = 'u';
            m.mtext[3] = '_';
            m.mtext[4]=CpuNo+1+'0';
            m.mtext[5]='\0';
            int sendcheck = msgsnd(ToProcQueue,&m,sizeof(m.mtext),!IPC_NOWAIT);
            if(sendcheck==-1)
                cout<<"\nsending failed";
            //signal process to continue
            sleep(2);
            kill(Cpu[CpuNo].process-> pid,SIGCONT);
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
    for(int i=0;i<2;i++){
        NewInserted[i].process=NULL;
        NewInserted[i].RemainingCycle=0;
        Cpu[i].RemainingCycle=0;
        Cpu[i].process=NULL;
        Switch[i].process=NULL;
        Switch[i].RemainingCycle=0;
    }
    IOs FIO;
    FIO.mtype = flg;
    FIO.RemainingCycle=0;
    IOs  FIORec;
    FIORec.mtype= flg;
    bool NI ;
    int sizercv;
    while(KilledProcNo!=TotProcessesNo)
    {   
        NI=false;
        
        while(*flaginserted==0);
            if(*flaginserted==-1){
                *flaginserted=0;
        }
        sizercv = msgrcv(FromIOQueue,&FIORec,sizeof(IOs),FIORec.mtype,!IPC_NOWAIT);
            if(sizercv == -1)
                cout << "\nreceiving failed!";
        if(FIORec.RemainingCycle>0||*flaginserted>0)
        {
            NI=true;
            for(int j = 0; j < 2; j++)
            {
                if(Cpu[j].process==NULL && Cpu[j].RemainingCycle > 0)
                    NewInserted[j].process = Switch[j].process;
                else
                    NewInserted[j].process = Cpu[j].process;
                if(Cpu[j].process!=NULL)
                {
                    Cpu[j].process->arr[Cpu[j].process->index].time--;
                    Cpu[j].RemainingCycle--;
                    if(Cpu[j].RemainingCycle==0)
                    {
                        NewInserted[j].process=NULL;
                    }
                }
            }
            Schd->InsertNewReady(&NewInserted[0],&NewInserted[1],&BlockedForMemory,MemoryAvailable,FIORec.RemainingCycle);
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
                    if(SendToIO[i]!=NULL){//send to IO
                        SendProcToIO(i,FIO);
                        FIO.RemainingCycle++;
                    }
                    
                    if(Switch[i].process == NULL)
                        PrepareProcess(i);
                    
                    RunProcess(i);
                }
                
            }
            else if(Cpu[i].process!=NULL)//if a process was running on the CPU
            {
                if(!NI)
                {
                    Cpu[i].process->arr[Cpu[i].process->index].time--;
                    Cpu[i].RemainingCycle--;
                }
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
        FIO.mtype=flg;
        printf("\nsending %hd as flag",FIO.RemainingCycle);
        int sendcheck = msgsnd(ToIOQueue,&FIO,sizeof(IOs),!IPC_NOWAIT);
        if(sendcheck==-1)
            cout<<"\nsending to io failed";
        FIO.RemainingCycle=0;
        //pause();
        raise(SIGSTOP);
    }
    ofstream MyFile;
    MyFile.open("CPU_1.txt",ios::app);   
    MyFile<<"\nCPU Utilization = "<<CpuUtaliz[0]/(float)((*Counter)-1);
    MyFile.close();
    
    MyFile.open("CPU_2.txt",ios::app);   
    MyFile<<"\nCPU Utilization = "<<CpuUtaliz[1]/(float)((*Counter)-1);
    MyFile.close();
    
    MyFile.open("Stat.txt",ios::trunc);
    //create the other file to write into **
    float AvgWTA=0,AvgWait=0,Std=0;
    for(int i = 0 ; i < TotProcessesNo ; i++)
    {
        AvgWTA+=KilledP[i].WTA;
        AvgWait+=KilledP[i].Wait;
        
    }
    AvgWTA=AvgWTA/(float)TotProcessesNo;
    AvgWait=AvgWait/(float)TotProcessesNo;
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
Dispatcher* Disp;
void Terminate(int);
int main(int argc, char** argv){
    int init=msgget(QKey,IPC_CREAT|0666);
    Config con;
    msgrcv(init,&con,sizeof(Config),0,!IPC_NOWAIT);
    msgctl(init,IPC_RMID,0);
    printf("\nDispatcher");
    Scheduler* Sched=NULL;
    switch(con.Numbers[1]){
        case(1):
                Sched= new RoundRobin(con.Numbers[3],con.Numbers[0]);
                break;
        case(2):
                Sched= new STRN();
                break;
        case(3):
                Sched= new HPF();
                break;
        case(4):
                Sched= new OurAlgo(con.Numbers[2]);
                break;
                        
    }
    Disp=new Dispatcher(con.Numbers[2],2000000,con.Numbers[0],Sched);
    Disp->Dispatch();
    delete Disp;
    kill(getpgrp(),SIGINT);
}
void Terminate(int){
    delete Disp;
}



