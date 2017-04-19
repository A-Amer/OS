#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include"Defines.h"


using namespace std;
void Terminate(int);
int shmid;
int main(int argc, char** argv) {
    signal(SIGINT,Terminate);
    shmid=shmget(CounterKey,sizeof(int),IPC_CREAT|0644);
    int* shmadd=(int*)shmat(shmid, (void *)0, 0);//attaching memory
    *shmadd=0;
    int PID = fork();
    if(PID==0){//start with Process Generator
        execve("ProcessGenerator",NULL,NULL);
    }
    sleep(500);
    PID=fork();
    if(PID==0){//start with Process Generator
        execve("Dispatch",NULL,NULL);
    }
    PID=fork();
    if(PID==0){//start with Process Generator
        execve("Device",NULL,NULL);
    }
    while(true){
        sleep(1000);
        *shmadd+=1;
        kill(getpgrp(),SIGCONT);
        
    }
    
    return 0;
}
void Terminate(int){
    shmctl(shmid, IPC_RMID, (shmid_ds*)0);	
    raise(SIGKILL);
}
