

#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include<cstdlib>
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
     void LinkedList()
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

void Delete(Node* p1,Node * p2)

{
    p1->next=p2->next;
    delete p2;
    count--;
}

};


#endif /* LINKEDLIST_H */

