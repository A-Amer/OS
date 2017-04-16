
#ifndef CPU_H
#define CPU_H
struct RunningTime
{

char type;
int time=0;


};

typedef struct Process //process parameters
{
    int size;

    struct RunningTime arr[20];
    int index;
    int pid;
    int priroity;
    int arrivaltime;

}Process;
typedef struct CPU { 
int RemainingCycle;
Process * process;
};


#endif /* CPU_H */

