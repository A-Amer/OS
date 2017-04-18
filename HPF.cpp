
#include "HPF.h"

void HPF:: InsertReady(Proc* process,int CurrTime){
    List.Insert(process);
}
void HPF::InsertNewReady(Proc * p1,Proc * p2){
                Proc *ptr;
		
		while (flag!=0)
		{
			List.Insert(ptr);
		}
}
CPUs HPF::Schedule(short CpuNo){
    CPUs C;
		
    if (List.head==NULL)
    {
            C.process = NULL;
            C.RemainingCycle= 0;
            return C;
    }
    else
    {
            C.process = List.head->p;
            C.RemainingCycle = HPF(List.head->p);
            return C;
    }
}
int HPF::HPF(Process *p){
    Node *temp =new Node;
		temp =List.head;
		int MaxPriorty = List.head->p->priroity;
		int i;
		while (temp!=List.tail)
		{
			if (MaxPriorty<temp->p->priroity)
			{
				MaxPriorty = temp->p->priroity;
				i = temp->p->index;
				temp = temp->next;
			}
		}
		int RunTime = temp->p->arr[i].time;
		return RunTime;
}

