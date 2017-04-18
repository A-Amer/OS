#include <cstdlib>
#include<sys/ipc.h>
#include<sys/msg.h>
#include <unistd.h>
#include"CPU.h"
#define BlockedQKey 3500
#define ToReadyQKey 4000
#define IO          0
#define Printer     1
#define CD          2
#define flag        3

using namespace std;
IOs DeviceArr[4];
Proc* Send[4];
int main() {
    IOs Next;
    int x,CountSend;
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
        msgrcv(RecvBuff,&x,sizeof(int),flag,!IPC_NOWAIT);
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
                    DeviceArr[i]=Next;
                    CountSend++;
                }
                else
                    DeviceArr[i].process=NULL;
            }
        }
        msgsnd(SendBuff,&CountSend,sizeof(int),!IPC_NOWAIT);
        for(int i=0;i<4;i++){
            if(Send[i]!=NULL)
                msgsnd(SendBuff,&Send[i],sizeof(Proc*),!IPC_NOWAIT);
        }
        
    }
    return 0;
}

