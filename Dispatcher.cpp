#include "Dispatcher.h"
#include"Defines.h"

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
    Schd = S;
}


Dispatcher::~Dispatcher() 
{
    delete Schd;
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
    m.mtext;//set**
    int sendflag = msgsnd(ToProcQueue,&m,sizeof(struct msgbuff),!IPC_NOWAIT);
    if(sendflag==-1)
        cout<<"\nsending failed";
    //signal process to continue
    kill(Cpu[CpuNo].process-> pid,SIGCONT);
    
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
                char* proc[4]={"Out",Pcount,Arrival,CpuNum,0};//check it
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
    SendInteger FIO;
    FIO.mtype = 1234;
    SendInteger FIORec;
    FIORec.mtype=5678;
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
    //delete from shared memory**
    
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



