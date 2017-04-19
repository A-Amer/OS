
#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H
#include"CPU.h"
    struct msgbuff{
	long mtype;
	char mtext[10];
    };
    struct KilledQ{
        long mtype;
        int Wait,WTA;
        
    };
    struct SendInteger{
        long mtype;
        int flag;
    };
    struct SendProc{
        long mtype;
        Proc* P;
    };
    typedef struct IoStruct { 
    long mtype;
    int RemainingCycle;
    Proc * process;
}IOs;


#endif /* MESSAGEQUEUE_H */

