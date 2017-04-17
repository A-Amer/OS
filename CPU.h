
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

}Proc;

typedef struct CPU { 
int RemainingCycle;
Proc * process;
}CPUs;


#endif /* CPU_H */

