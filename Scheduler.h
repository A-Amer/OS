
#ifndef SCHEDULER_H
#define SCHEDULER_H
#include"CPU.h"
#include <functional>
#include <queue>
#include <vector>
using namespace std;


typedef struct RunProcess{
    bool IsCpu=false;
    float priority;
    short CpuTime;//cpu time currently
    short ArrTime;
    Process * process;

}RunProc;
class Scheduler{
public:
   virtual void InsertReady(Process * process);
   virtual CPU Schedule(); 
    
};



#endif /* SCHEDULER_H */
