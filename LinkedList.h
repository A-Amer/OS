

#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include<cstdlib>
#include"CPU.h"
    struct Node
    {

        Proc *p;
        Node *next;

    };

    class LinkedList
    {
     
    public:
        Node *head;
        Node *tail;
        int count;
    public:
     LinkedList()
    {
         head=NULL;
        tail=NULL;
        count=0;
     }

    void Insert(Proc *p1)
    {
        if(head==NULL)
        {
        head=new Node;
        head->p=p1;
        head->next=NULL;
        tail=head;
        count++;
        return;
    }
    else
    {
        tail->next=new Node;
        tail=tail->next;
        tail->next=NULL;
        tail->p=p1;
        count++;
    }
    return;
}
    void DeleteHead()
{
    if(count==1)
    {
        delete head;
        head=NULL;
        tail=NULL;
    }
    else
    {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
    count--;
}

void Delete(Node* p1,Node * p2)

{
    if(p2==head){
        head=p2->next;
    }
    else if(p2==tail){
        Node*temp=head;
        while(temp->next->next!=NULL){
            temp=temp->next;
        }
        tail=temp;
        temp->next=NULL;
    }
    else{
        p1->next=p2->next;
    }
    delete p2;
    count--;
}

};


#endif /* LINKEDLIST_H */

