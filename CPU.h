
#ifndef CPU_H
#define CPU_H


struct RunningTime
{

char type;
int time;


};

typedef struct Process //process parameters
{
    int size;

    struct RunningTime arr[20];
    int index;
    int pid;
    int priroity;
    int arrivaltime;
    int Pnum;
    
}Proc;

typedef struct CPU { 
int RemainingCycle;
Proc * process;
}CPUs;
typedef struct ConfigFile{
    long  mtype;
    int Numbers[5];
}Config;



#endif /* CPU_H */

