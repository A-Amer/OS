#include <cstdlib>
#include <stdio.h>
#include<iostream>
#include <fstream>
#include<signal.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include <unistd.h>
#include"MessageQueue.h"

#define QKey 2000
#define SharedQ 0x500

using namespace std;
void Stop(int);//see if signal could send int
void Continu(int);
void Terminate(int);

int count=1,PrevFinish=0,RecBuff,Arrival,Wait=0;
int ProcNum;
string FileName="Process_";
ofstream MyFile;
char WorkOn[15];
int main(int argc, char** argv) {
    RecBuff=msgget(QKey,IPC_CREAT|0646);
    ProcNum=atoi(argv[1]);
    FileName.append(argv[1]);
    FileName.append(".txt");
    Arrival=atoi(argv[2]);
    WorkOn=argv[3];
    wait+=(count-Arrival);
    //////////////////////////////
    signal(SIGTSTP,Stop);
    signal(SIGCONT,Continu);
    signal(SIGINT,Terminate);
    /////////////////////////////////
    if(WorkOn=="CPU_1"){
        MyFile.open("CPU_1.txt",ios::trunc); 
    }
    else{
        MyFile.open("CPU_2.txt",ios::trunc);
    }
    MyFile<<"["<<count<<":";
    MyFile.close();
    MyFile.open(FileName,ios::trunc);
    MyFile<<"["<<count<<":";
    count++;
    MyFile.close();
    while(true);
    return 0;
}
void Stop(int dummy){
    if(WorkOn=="CPU_1"){
        MyFile.open("CPU_1.txt",ios::app); 
        MyFile<<count<<"] Process_"<<ProcNum<<"\n";
        MyFile.close();
    }
    else if(WorkOn=="CPU_2"){
        MyFile.open("CPU_2.txt",ios::app);
        MyFile<<count<<"] Process_"<<ProcNum<<"\n";
        MyFile.close();
    }
   
    MyFile.open(FileName,ios::app);
    MyFile<<count<<"]"<<WorkOn<<"\n";
    MyFile.close();
    PrevFinish=count;
    count++;
    raise(SIGSTOP);
}
void Continu(int dummy){
    struct msgbuff m;
    m.mtype=getpid();
    msgrcv(RecBuff,&m,sizeof(struct msgbuff),getpid(),!IPC_NOWAIT);
    WorkOn=m.mtext;
    if(WorkOn=="CPU_1"){
        MyFile.open("CPU_1.txt",ios::app); 
        MyFile<<"["<<count<<":";
        MyFile.close();
    }
    else if(WorkOn=="CPU_2"){
        MyFile.open("CPU_2.txt",ios::app);
        MyFile<<"["<<count<<":";
        MyFile.close();
    }
    MyFile.open(FileName,ios::app);
    MyFile<<"["<<count<<":";
    MyFile.close();
    Wait+=(count-PrevFinish);
    count++;
}
void Terminate(int dummy){
    int TurnAround=count-Arrival;
    MyFile.open(FileName,ios::app);
    MyFile<<"turn_around ="<<TurnAround<<"\n";
    TurnAround=TurnAround/(TurnAround-Wait);
    MyFile<<"weighted_turn_around = "<<TurnAround<<"\n";
    MyFile.close();
    struct KilledQ exit;
    exit.WTA=TurnAround;
    exit.Wait=Wait;
    int shmid = shmget(SharedQ, sizeof(KilledQ), IPC_CREAT|0644);
    KilledQ* AddKilled=(KilledQ*)shmat(shmid, (void *)0, 0);
    AddKilled[ProcNum]=exit;
    shmdt(AddKilled);
    raise(SIGKILL);
}

