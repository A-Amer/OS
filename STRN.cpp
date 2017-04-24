#include<iostream>
#include "STRN.h"
#include "MessageQueue.h"
using namespace std;

void STRN:: InsertReady(Proc* process){
    l.Insert(process);
}
CPUs STRN::Schedule(short CpuNo){
    CPUs c;
    if(l.head==NULL){
        c.RemainingCycle=0;
        c.process=NULL;
        return c;
    }
    int Locindex=l.head->p->index;
    int RunTime=l.head->p->arr[Locindex].time;
    int min=RunTime;
    if(l.count==0)
    {
        c.process=NULL;

        c.RemainingCycle=0;
        return c;

    }

    if(l.count==1)
    {  
        c.process=l.head->p;
        c.RemainingCycle=min;
        delete l.head;
        l.head=NULL;
        l.tail=NULL;
        l.count--;
        return c;

    } 
    Node *ptr1=l.head;
    Node *ptr2=l.head->next;
    Node* minimumpointer1=l.head;
    Node *minimumpointer2=l.head->next;

    for(int i=1;i<l.count;i++)
            {

                    index=ptr2->p->index;
                    RunTime=ptr2->p->arr[index].time;
                    if(RunTime<min)
                    {
                    min=RunTime;
                    minimumpointer1=ptr1;
                    minimumpointer2=ptr2;
                    }
                    ptr1=ptr2;
                    ptr2=ptr2->next;
            }
    c.process=minimumpointer2->p;
    c.RemainingCycle=min;
    l.Delete(minimumpointer1,minimumpointer2);

   return c;
}

void STRN::InsertNewReady(CPU * c1, CPU * c2, LinkedList * BlockedList, int MemoryAvailable,int IOReturnNo)
{
    Node * P1= NULL;
    Node * P2= NULL;
    
    //Proc *ptr=ProcessArr+index+1;
    
    int minimum1=0;
    int minimum2=0;
    int temp=*flag;
    LinkedList ChooseProc;
    ChooseProc.head=NULL;
    ChooseProc.tail=NULL;
    ChooseProc.count=0;
    
    
    while(temp > 0)
    {
        if((ProcessArr+index+1)->size>MemoryAvailable)
            BlockedList->Insert((ProcessArr+index+1));
        else
            ChooseProc.Insert((ProcessArr+index+1));
        temp--;
        index++;
       // ptr++;
    }
    IOs device;//change this to proc struct
    int sizercv;
    Node * s=ChooseProc.head;
    while(IOReturnNo != 0)
    {
        sizercv = msgrcv(FromIOQueue,&device,sizeof(IOs),0,!IPC_NOWAIT);
            if(sizercv == -1)
                cout << "\nreceiving failed!";
        int j=0;
        while((ProcessArr+j)-> pid != device.process)
            j++;
        ProcessArr[j].index++;
        ChooseProc.head=s;
        ChooseProc.Insert(ProcessArr+j);
        IOReturnNo--;
        s=ChooseProc.head;
    }
    
    if(c1->process!=NULL)
        ChooseProc.Insert(c1->process);

    if(c2->process!=NULL)
        ChooseProc.Insert(c2->process);
    
    Node * loopptr;
    if(ChooseProc.count == 1)
    {
        P1 = ChooseProc.head;
        minimum1 = P1->p->arr[P1->p->index].time;       
    }
    
    else if(ChooseProc.count >1)
    {
        P1 = ChooseProc.head;
        minimum1 = P1->p->arr[P1->p->index].time;
        P2 = ChooseProc.head->next;
        minimum2 = P2->p->arr[P2->p->index].time;
        loopptr = P2->next;
        while(loopptr!=NULL)
        {
            temp = loopptr->p->arr[loopptr->p->index].time;
            if(temp<minimum1 && minimum1>=minimum2)
            {
                InsertReady(P1->p);
                minimum1 = temp;
                P1 = loopptr;
            }
            else if (temp<minimum2 && minimum2>minimum1)
            {
                InsertReady(P2->p);
                minimum2 = temp;
                P2 = loopptr;
            }
            loopptr = loopptr->next;
        }
    }
    if(P1==NULL)
        return;
    if(P2==NULL)
    {
        if(c1->process!=NULL)
            c1->process=NULL;
        else if (c2->process!=NULL)
            c2->process=NULL;
        else
        {
            c1->process=P1->p;
            c1->RemainingCycle=minimum1;
        }
        return;
    }
    if(c1->process==NULL&&c2->process==NULL)
    {
        c1->process=P1->p;
        c1->RemainingCycle=minimum1;
        c2->process=P2->p;
        c2->RemainingCycle=minimum2;
        return;
    }
    
    else if(c2->process==P1->p)
    {
        c2->process=NULL;
        if(c1->process==P2->p)
        {
            c1->process=NULL;
            return;
        }
        c1->process=P2->p;
        c1->RemainingCycle = minimum2;
        return;
    }
    else if(c2->process==P2->p)
    {
        c2->process=NULL;
        if(c1->process==P1->p)
        {
            c1->process=NULL;
            return;
        }
        c1->process=P1->p;
        c1->RemainingCycle = minimum1;
        return;
    }
    else if(c1->process==P1->p)
    {
        c1->process=NULL;
        if(c2->process==P2->p)
        {
            c2->process=NULL;
            return;
        }
        c2->process=P2->p;
        c2->RemainingCycle = minimum2;
        return;
    }
    else if(c1->process==P2->p)
    {
        c1->process=NULL;
        if(c2->process==P1->p)
        {
            c2->process=NULL;
            return;
        }
        c2->process=P1->p;
        c2->RemainingCycle = minimum1;
        return;
    }
    c1->process=P1->p;
    c1->RemainingCycle=minimum1;
    c2->process=P2->p;
    c2->RemainingCycle=minimum2;
    return;
    
}

