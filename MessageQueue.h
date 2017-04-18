
#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H
    struct msgbuff{
	long mtype;
	char mtext[10];
    };
    struct KilledQ{
        int Wait,WTA;
        
    };


#endif /* MESSAGEQUEUE_H */

