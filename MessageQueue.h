
#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H
#include"CPU.h"
    struct msgbuff{
	long mtype;
	char mtext[10];
    };
    struct KilledQ{
        int Wait,WTA;
        
    };
    struct SendProc{
        long mtype;
        Proc * process;
        
    };
    typedef struct IoStruct { 
    long mtype;
    short RemainingCycle;
    int process;

}IOs;


#endif /* MESSAGEQUEUE_H */

