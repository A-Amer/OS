#include <cstdlib>
#include<sys/ipc.h>
#include<sys/msg.h>
#include <unistd.h>
#include<signal.h>
#include"CPU.h"
#include "MessageQueue.h"
#include"Defines.h"

using namespace std;

IOs DeviceArr[4];
Proc* Send[4];
void Terminate(int);

int main() {
    IOs Next;
    int x,CountSend;
    signal(SIGINT,Terminate);
    int RecvBuff=msgget(BlockedQKey,IPC_CREAT|0646);
    int SendBuff=msgget(ToReadyQKey,IPC_CREAT|0646);
    for(int i=0;i<4;i++){
        DeviceArr[i].RemainingCycle=0;
        DeviceArr[i].process=NULL;
    }
    while(1){
        for(int i=0;i<4;i++){
            DeviceArr[i].RemainingCycle--;
        }
        CountSend=0;
        //need to wait for flag
        SendInteger Intg;
        
        msgrcv(RecvBuff,&Intg,sizeof(SendInteger),flg,!IPC_NOWAIT);
        for(int i=0;i<4;i++){
            if(DeviceArr[i].process==NULL||DeviceArr[i].RemainingCycle<=0){
                Send[i]=DeviceArr[i].process;
                if(i==0||i==1){
                    Next.mtype=IO;
                    x=msgrcv(RecvBuff,&Next,sizeof( IOs),IO,IPC_NOWAIT);
                }
                
                else if(i==2){
                    Next.mtype=Printer;
                    x=msgrcv(RecvBuff,&Next,sizeof( IOs),Printer,IPC_NOWAIT);
                }
                else{
                    Next.mtype=CD;
                    x=msgrcv(RecvBuff,&Next,sizeof( IOs),CD,IPC_NOWAIT);
                }
                if(x!=-1){
                    Next.RemainingCycle=Next.process->arr[Next.process->index];
                    DeviceArr[i]=Next;
                    CountSend++;
                }
                else
                    DeviceArr[i].process=NULL;
            }
        }
        Intg.flag=CountSend;
        Intg.mtype=flg;
        msgsnd(SendBuff,&Intg,sizeof(SendInteger),!IPC_NOWAIT);
        for(int i=0;i<4;i++){
            if(Send[i]!=NULL){
                SendProc Snd;
                Snd.P=Send[i];
                msgsnd(SendBuff,&Snd,sizeof(SendProc),!IPC_NOWAIT);
            }
        }
      pause();  
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

