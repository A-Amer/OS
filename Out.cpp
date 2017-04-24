#include <cstdlib>
#include <stdio.h>
#include<iostream>
#include<string>
#include <fstream>
#include<signal.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include <unistd.h>
#include"MessageQueue.h"
#include"Defines.h"

using namespace std;
void Stop(int);//see if signal could send int
void Continu(int);
void Terminate(int);


int* count;
int PrevFinish=0,RecBuff,Arrival,Wait=0;
int ProcNum;
string FileName="Process_";
ofstream MyFile;
string WorkOn;
int main(int argc, char** argv) {
   
    ProcNum=atoi(argv[1]);
    FileName.append(argv[1]);
    FileName.append(".txt");
    Arrival=atoi(argv[2]);
    WorkOn=argv[3];
    
    //////////////////////////////
    int shmid=shmget(CounterKey,sizeof(int),IPC_CREAT|0644);
    count=(int*)shmat(shmid, (void *)0, 0);
    int now=*count;
    Wait+=(now-Arrival);
    //////////////////////////////
    signal(SIGTSTP,Stop);
    signal(SIGCONT,Continu);
    signal(SIGINT,Terminate);
    /////////////////////////////////
    if(WorkOn[0]=='c'&&WorkOn[4]=='1'){
        MyFile.open("CPU_1.txt",ios::app);  
        MyFile<<"["<<now<<":";
        MyFile.close();
    }
    else if(WorkOn[0]=='c'&&WorkOn[4]=='2'){
        MyFile.open("CPU_2.txt",ios::app);
        MyFile<<"["<<now<<":";
        MyFile.close();
    }
    
    MyFile.open(FileName.c_str(),ios::trunc);
    MyFile<<"["<<now<<":";
    MyFile.close();

    while(true);
    return 0;
}
void Stop(int dummy){
    int now=*count;
    if(WorkOn[0]=='c'&&WorkOn[4]=='1'){
        MyFile.open("CPU_1.txt",ios::app); 
        MyFile<<now<<"] Process_"<<ProcNum<<"\n";
        MyFile.close();
    }
    else if(WorkOn[0]=='c'&&WorkOn[4]=='2'){
        MyFile.open("CPU_2.txt",ios::app);
        MyFile<<now<<"] Process_"<<ProcNum<<"\n";
        MyFile.close();
    }
    MyFile.open(FileName.c_str(),ios::app);
    MyFile<<now<<"]"<<WorkOn<<"\n";
    MyFile.close();
    PrevFinish=now;

    raise(SIGSTOP);
}
void Continu(int dummy){
    int now=*count;
    struct msgbuff m;
    RecBuff=msgget(QKey,IPC_CREAT|0666);
   int x=msgrcv(RecBuff,&m,sizeof(struct msgbuff),getpid(),!IPC_NOWAIT);

   if(x!=-1){
    if(m.mtext[0]==IO){
        WorkOn="IO\0";
    }
    else if(m.mtext[0]==Printer){
        WorkOn="printer\0";
    }
    else if(m.mtext[0]==CD){
        WorkOn="CD\0";
    }
    else{
        WorkOn=m.mtext;
    }
    if(WorkOn[0]=='c'&& WorkOn[4]=='1'){
        MyFile.open("CPU_1.txt",ios::app); 
        MyFile<<"["<<now<<":";
        MyFile.close();
    }
    else if(WorkOn[0]=='c'&& WorkOn[4]=='2'){
        MyFile.open("CPU_2.txt",ios::app);
        MyFile<<"["<<now<<":";
        MyFile.close();
    }
    MyFile.open(FileName.c_str(),ios::app);
    MyFile<<"["<<now<<":";
    MyFile.close();
    Wait+=(now-PrevFinish);
   }

}
void Terminate(int dummy){
    int now=*count;
    if(WorkOn[0]=='c'&& WorkOn[4]=='1'){
        MyFile.open("CPU_1.txt",ios::app); 
        MyFile<<now<<"] Process_"<<ProcNum<<"\n";
        MyFile.close();
    }
    else if(WorkOn[0]=='c'&& WorkOn[4]=='2'){
        MyFile.open("CPU_2.txt",ios::app);
        MyFile<<now<<"] Process_"<<ProcNum<<"\n";
        MyFile.close();
    }
   
    MyFile.open(FileName.c_str(),ios::app);
    MyFile<<now<<"]"<<WorkOn<<"\n";
    MyFile.close();
    int TurnAround=now-Arrival;
    MyFile.open(FileName.c_str(),ios::app);
    MyFile<<"turn_around ="<<TurnAround<<"\n";
    MyFile<<"waiting="<<Wait<<"\n";
    float WTA=TurnAround/(float)(TurnAround-Wait);
    MyFile<<"weighted_turn_around = "<<WTA<<"\n";
    MyFile.close();
    int shmid = shmget(SharedQ, sizeof(KilledQ), IPC_CREAT|0644);
    KilledQ* AddKilled=(KilledQ*)shmat(shmid, (void *)0, 0);
    AddKilled[ProcNum].Wait=Wait;
    AddKilled[ProcNum].WTA=WTA;
    shmdt(AddKilled);
    shmdt(count);

    raise(SIGKILL);
}

