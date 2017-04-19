#include "HPF.h"

void HPF:: InsertReady(Proc* process){
    List.Insert(process);
}
void HPF::InsertNewReady(CPU c1,CPU c2, LinkedList BlockedList, int MemoryAvailable,int IOReturnNo){
                Proc *ptr;
		
		for(int i=1;i<=*flag;i++){
                    List.Insert(ProcessArr+index+i);
                }
                index+=*flag;
                *flag=0;
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
            C.RemainingCycle = HiPF(C.process);
            return C;
    }
}
int HPF::HiPF(Process* &p){
    
		Node * temp =List.head;
                Node* max=temp;
		int MaxPriorty = List.head->p->priroity;
		int i;
		while (temp!=NULL)
		{
			if (MaxPriorty<temp->p->priroity)
			{
				MaxPriorty = temp->p->priroity;
                                max=temp;
				i = temp->p->index;
				temp = temp->next;
			}
		}
                p=max->p;
                temp =List.head;
                while (temp->next!=NULL){
                    if(temp->next==max){
                        break;
                    }
                    temp=temp->next;
                }
                //delete max
                List.Delete(temp,max);
		int RunTime = p->arr[p->index].time;
		return RunTime;
}

