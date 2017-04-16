
#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H
    struct msgbuff{
	long mtype;
	char mtext[10];
    };
    struct KilledQ{
        int wait,WTA;
        
    };


#endif /* MESSAGEQUEUE_H */

