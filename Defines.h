#ifndef DEFINES_H
#define DEFINES_H
#define CounterKey 0x8000//global counter
#define BlockedQKey 3500//IO blocked queue
#define ToReadyQKey 4000//Queue of processes returned form IO
#define IO          'I'//message id for io
#define Printer     'p'//message id for printer
#define CD          'C' //message id for cd
#define flg         3//message id for flag
#define QKey        2000//Key for message queue between processes and dispatcher
#define SharedQ     0x500// done processes(for statistics)
#define FlagKey     0x200//When a new process is added
#define ProcArrKey  0x12345//Processes added by process generator
#define SemKey      200
#endif /* DEFINES_H */

