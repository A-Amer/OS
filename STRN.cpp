

#include "STRN.h"
void STRN:: InsertReady(Proc* process,int CurrTime){
    l.Insert(process);
}
CPUs STRN::Schedule(short CpuNo){
    CPUs c;
    int index=l.head->p->index;
    int RunTime=l.head->p->arr[index].time;
    int min=RunTime;
    if(count==0)
    {
        c.process=NULL;

        c.RemainingCycle=0;
        return c;

    }

    if(count==1)
    {  
        c.process=l.head;
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

void STRN::InsertNewReady(Proc * p1,Proc * p2)
{
	Proc *ptr=ptail-flag;
        int insertionFlag=flag;
	int minimum1,minimum2,temp;
	int pi;//ptr index
	int i1,i2,rt1,rt2; //current process index&runtime
	int rt;//runtime
	Proc * pt1;
	Proc *pt2;
Proc *ptemp;

	i1=p1->index;
	i2=p2->index;
	rt1=p1->arr[i1].time;
	rt2=p2->arr[i2].time;

	pi=ptr->index;

	minimum1=ptr->arr[pi].time;
	pt1=ptr;
	if(flag==1)
	minimum2=-1;
	else
	{
	ptr++;
	pi=ptr->index;
	minimum2=ptr->arr[pi].time;
	pt2=ptr;
	ptr++;
	}
	flag--;
	flag--;
	if(minimum2!=-1)
	{
		if(minimum2<minimum1)
		{
		 temp=minimum1;
		minimum1=minimum2;
		minimum2=temp;
                ptemp=pt1;
		pt1=pt2;
		pt2=pt1;

		}


	}

	while(flag>0)
	{
		pi=ptr->index;
		rt=ptr->arr[pi].time;
		if(rt<minimum1)
		{
		
		minimum2=minimum1;
		minimum1=rt;
		pt2=pt1;
		pt1=ptr;

		}
		else if(rt<minimum2)
		{
		minimum2=rt;
		pt2=ptr;


		}

		ptr++;
		flag--;


	}


	//CHOOSING WHICH WILL STAY FROM CURRENT    //remains insertion
	if(minimum1<rt1)
	{
		p1=pt1;
		if(minimum2<rt2 && minimum2!=-1)
		{
			p2=pt2;
		      ptr=ptail-insertionFlag;
		       insertionFlag= insertionFlag-2;  // aLL INSERTED EXCEPT 2
		      while(insertionFlag>0)
			{
			       if(ptr!=p1 && ptr!=p2)
				   {
				l.Insert(ptr);
				ptr++;

				insertionFlag--;

				    }
                                ptr++;
			  }
		}

		else   //all except 1
		{
		       p2=NULL;

                       ptr=ptail-insertionFlag;
		       insertionFlag= insertionFlag-1;  // aLL INSERTED EXCEPT 1
		      while(insertionFlag>0)
			{
			       if(ptr!=p1)
				   {
				l.Insert(ptr);
				ptr++;

				insertionFlag--;

				    }
                                ptr++;
			  }
		}
	}
	else if(minimum1<rt2)
	{
      
		p2=pt1;
                  ptr=ptail-insertionFlag;
		       insertionFlag= insertionFlag-1;  // aLL INSERTED EXCEPT 1
		      while(insertionFlag>0)
			{
			       if(ptr!=p2)
				   {
				l.Insert(ptr);
				ptr++;

				insertionFlag--;

				    }
                                ptr++;
			  }

		p1=NULL;
	}
     else
	{
         //if both remain NULL so insert all new into ready queue
            ptr=ptail-insertionFlag;
           while(insertionFlag>0)
             {

             l.Insert(ptr);
             ptr++;

             insertionFlag--;

             }
             p1=NULL;
             p2=NULL;
             }
}




