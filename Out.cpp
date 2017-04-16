#include <cstdlib>
#include <stdio.h>
#include<iostream>
#include <fstream>
#include<signal.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include <unistd.h>
#include"MessageQueue.h"

using namespace std;
void Stop(int);//see if signal could send int
void Continu(int);
void Terminate(int);

int count=1/*instead of counter for now*/,PrevFinish=0,RecBuff,Arrival,wait=0;
char FileName[15];
ofstream MyFile;

int main(int argc, char** argv) {
    RecBuff=msgget(2000,IPC_CREAT|0646);
    struct msgbuff m;
    m.mtype=getpid();
    argc=msgrcv(RecBuff,&m,sizeof(struct msgbuff),getpid(),!IPC_NOWAIT);//could be changed to arrgument
    sprintf(FileName, "Process_%d.txt", argc);
    Arrival=msgrcv(RecBuff,&m,sizeof(struct msgbuff),getpid(),!IPC_NOWAIT);
    wait+=(count-Arrival);
    signal(SIGTSTP,Stop);
    signal(SIGCONT,Continu);
    signal(SIGINT,Terminate);
    MyFile.open(FileName,ios::trunc);
    MyFile<<"["<<count<<":";
    count++;
    MyFile.close();
    while(true);
    return 0;
}
void Stop(int dummy){
    int Err=-1;
    struct msgbuff m;
    m.mtype=getpid();
    while(Err==-1){
    Err=msgrcv(RecBuff,&m,sizeof(struct msgbuff),getpid(),!IPC_NOWAIT);
    }
    MyFile.open(FileName,ios::app);
    MyFile<<count<<"]"<<m.mtext<<"\n";
    MyFile.close();
    PrevFinish=count;
    count++;
    raise(SIGSTOP);
}
void Continu(int dummy){
    MyFile.open(FileName,ios::app);
    MyFile<<"["<<count<<":";
    MyFile.close();
    wait+=(count-PrevFinish);
    count++;
}
void Terminate(int dummy){
    int TurnAround=count-Arrival;
    MyFile.open(FileName,ios::app);
    MyFile<<"turn_around ="<<TurnAround<<"\n";
    TurnAround=TurnAround/(TurnAround-wait);
    MyFile<<"weighted_turn_around = "<<TurnAround<<"\n";
    MyFile.close();
    struct KilledQ exit;
    exit.WTA=TurnAround;
    exit.wait=wait;
    msgrcv(RecBuff,&exit,sizeof(struct KilledQ),getpid(),!IPC_NOWAIT);//will be changed to shared memory
    raise(SIGKILL);
}

