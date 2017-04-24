#include <cstdlib>
#include<stdio.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include <unistd.h>
#include<signal.h>
#include"CPU.h"
#include "MessageQueue.h"
#include"Defines.h"

using namespace std;

IOs DeviceArr[4];
int Send[4];
void Terminate(int);

int main(int argc, char** argv) {
    IOs Next;
    int x,CountSend;
    signal(SIGINT,Terminate);
    printf("\nI am device");
    int RecvBuff=msgget(BlockedQKey,IPC_CREAT|0646);
    int SendBuff=msgget(ToReadyQKey,IPC_CREAT|0646);
    for(int i=0;i<4;i++){
        DeviceArr[i].RemainingCycle=0;
        DeviceArr[i].process=0;
    }
    while(1){
        for(int i=0;i<4;i++){
            if(DeviceArr[i].process!=0){
                DeviceArr[i].RemainingCycle--;
                
            }
        }
        CountSend=0;
        for(int i=0;i<4;i++){
            if(DeviceArr[i].process!=0 && DeviceArr[i].RemainingCycle==0){
                CountSend++;
                Send[i]=DeviceArr[i].process;
                DeviceArr[i].process=0;
            }
        }
        //need to wait for flag
        IOs Intg;
        Intg.RemainingCycle=CountSend;
        Intg.mtype=flg;
        msgsnd(SendBuff,&Intg,sizeof(IOs),!IPC_NOWAIT);
        for(int i=0;i<4;i++){
            if(Send[i]!=0){
                
                IOs Snd;
                Snd.process=Send[i];
                Snd.RemainingCycle=0;
                Snd.mtype=flg;
                msgsnd(SendBuff,&Snd,sizeof(IOs),!IPC_NOWAIT);
                kill(Send[i],SIGTSTP);  
                Send[i]=0;
            }
        }
        msgrcv(RecvBuff,&Intg,sizeof(IOs),flg,!IPC_NOWAIT);
        for(int i=0;i<4;i++){
            if(DeviceArr[i].process==0){
                
                if(i==0||i==1){
                    
                    x=msgrcv(RecvBuff,&Next,sizeof( IOs),IO,IPC_NOWAIT);
                }
                
                else if(i==2){
                    
                    x=msgrcv(RecvBuff,&Next,sizeof( IOs),Printer,IPC_NOWAIT);
                }
                else{
                    
                    x=msgrcv(RecvBuff,&Next,sizeof( IOs),CD,IPC_NOWAIT);
                }
                if(x!=-1){
                    printf("\nYes!");
                    DeviceArr[i].process=Next.process;
                    DeviceArr[i].RemainingCycle=Next.RemainingCycle;
                    kill(Next.process,SIGCONT);
                    printf("\nRecieved Process with PId=%d",DeviceArr[i].process);
                }
               
            }
            }

     raise(SIGSTOP); 
    }
    return 0;
}
void Terminate(int){
    int id=msgget(BlockedQKey,IPC_CREAT|0646);
    msgctl(id,IPC_RMID,0);
    id=msgget(ToReadyQKey,IPC_CREAT|0646);
    msgctl(id,IPC_RMID,0);
    raise(SIGKILL);
}

