#include<iostream>
#include "STRN.h"
#include "MessageQueue.h"
using namespace std;

void STRN:: InsertReady(Proc* process){
    l.Insert(process);
}
CPUs STRN::Schedule(short CpuNo){
    CPUs c;
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

//void STRN::InsertNewReady(CPU c1,CPU c2, LinkedList BlockedList)
//{
//        Proc * p1 = c1.process;
//        Proc * p2 = c2.process;     
//	Proc *ptr=ptail-flag;
//        int insertionFlag=flag;
//	int minimum1,minimum2,temp;
//	int pi;//ptr index
//	int i1,i2,rt1,rt2; //current process index&runtime
//	int rt;//runtime
//	Proc * pt1;
//	Proc *pt2;
//Proc *ptemp;
//
//	i1=p1->index;
//	i2=p2->index;
//	rt1=p1->arr[i1].time;
//	rt2=p2->arr[i2].time;
//
//	pi=ptr->index;
//
//	minimum1=ptr->arr[pi].time;
//	pt1=ptr;
//	if(flag==1)
//	minimum2=-1;
//	else
//	{
//	ptr++;
//	pi=ptr->index;
//	minimum2=ptr->arr[pi].time;
//	pt2=ptr;
//	ptr++;
//	}
//	flag--;
//	flag--;
//	if(minimum2!=-1)
//	{
//		if(minimum2<minimum1)
//		{
//		 temp=minimum1;
//		minimum1=minimum2;
//		minimum2=temp;
//                ptemp=pt1;
//		pt1=pt2;
//		pt2=pt1;
//
//		}
//
//
//	}
//
//	while(flag>0)
//	{
//		pi=ptr->index;
//		rt=ptr->arr[pi].time;
//		if(rt<minimum1)
//		{
//		
//		minimum2=minimum1;
//		minimum1=rt;
//		pt2=pt1;
//		pt1=ptr;
//
//		}
//		else if(rt<minimum2)
//		{
//		minimum2=rt;
//		pt2=ptr;
//		}
//		ptr++;
//		flag--;
//	}
//
//	//CHOOSING WHICH WILL STAY FROM CURRENT    //remains insertion
//	if(minimum1<rt1)
//	{
//		p1=pt1;
//		if(minimum2<rt2 && minimum2!=-1)
//		{
//			p2=pt2;
//		      ptr=ptail-insertionFlag;
//		       insertionFlag= insertionFlag-2;  // aLL INSERTED EXCEPT 2
//		      while(insertionFlag>0)
//			{
//			       if(ptr!=p1 && ptr!=p2)
//				   {
//				l.Insert(ptr);
//				ptr++;
//
//				insertionFlag--;
//
//				    }
//                                ptr++;
//			  }
//		}
//
//		else   //all except 1
//		{
//		       p2=NULL;
//
//                       ptr=ptail-insertionFlag;
//		       insertionFlag= insertionFlag-1;  // aLL INSERTED EXCEPT 1
//		      while(insertionFlag>0)
//			{
//			       if(ptr!=p1)
//				   {
//				l.Insert(ptr);
//				ptr++;
//
//				insertionFlag--;
//
//				    }
//                                ptr++;
//			  }
//		}
//	}
//	else if(minimum1<rt2)
//	{
//      
//		p2=pt1;
//                  ptr=ptail-insertionFlag;
//		       insertionFlag= insertionFlag-1;  // aLL INSERTED EXCEPT 1
//		      while(insertionFlag>0)
//			{
//			       if(ptr!=p2)
//				   {
//				l.Insert(ptr);
//				ptr++;
//
//				insertionFlag--;
//
//				    }
//                                ptr++;
//			  }
//
//		p1=NULL;
//	}
//     else
//	{
//         //if both remain NULL so insert all new into ready queue
//            ptr=ptail-insertionFlag;
//           while(insertionFlag>0)
//             {
//
//             l.Insert(ptr);
//             ptr++;
//
//             insertionFlag--;
//
//             }
//             p1=NULL;
//             p2=NULL;
//             }
//}

void STRN::InsertNewReady(CPU c1, CPU c2, LinkedList BlockedList, int MemoryAvailable,int IOReturnNo)
{
    Node * P1= NULL;
    Node * P2= NULL;
    Proc *ptr=ProcessArr+index+1;
    
    int minimum1=0;
    int minimum2=0;
    int temp;
    LinkedList ChooseProc;
    
    while(*flag != 0)
    {
        if(ptr->size>MemoryAvailable)
            BlockedList.Insert(ptr);
        else
            ChooseProc.Insert(ptr);
        *flag--;
        index++;
        ptr++;
    }
    IOs device;//change this to proc struct
    int sizercv;
    while(IOReturnNo != 0)
    {
        sizercv = msgrcv(FromIOQueue,&device,sizeof(IOs),0,!IPC_NOWAIT);
            if(sizercv == -1)
                cout << "\nreceiving failed!";
        ChooseProc.Insert(device.process);
        IOReturnNo--;
    }
    
    if(c1.process!=NULL)
        ChooseProc.Insert(c1.process);

    if(c2.process!=NULL)
        ChooseProc.Insert(c2.process);
    
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
    if(c1.process==NULL&&c2.process==NULL)
    {
        c1.process=P1->p;
        c1.RemainingCycle=minimum1;
        c2.process=P2->p;
        c2.RemainingCycle=minimum2;
        return;
    }
    else if(P2->p==NULL)
    {
        c1.process=NULL;
        c2.process=NULL;
        return;
    }
    else if(c2.process==P1->p)
    {
        c2.process=NULL;
        c1.process=P2->p;
        c1.RemainingCycle = minimum2;
        return;
    }
    else if(c2.process==P2->p)
    {
        c2.process=NULL;
        c1.process=P1->p;
        c1.RemainingCycle = minimum1;
        return;
    }
    else if(c1.process==P1->p)
    {
        c1.process=NULL;
        c2.process=P2->p;
        c2.RemainingCycle = minimum2;
        return;
    }
    else if(c1.process==P2->p)
    {
        c1.process=NULL;
        c2.process=P1->p;
        c2.RemainingCycle = minimum1;
        return;
    }
    c1.process=P1->p;
    c1.RemainingCycle=minimum1;
    c2.process=P2->p;
    c2.RemainingCycle=minimum2;
    return;
    
}

