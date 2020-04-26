#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"// Process

typedef enum policy{
	FCFS,
	RR,
	SJF,
	PSJF	
}Policy;

void setCore(pid_t, int);
void schedule_FCFS(Process[], int);
void schedule_RR(Process[], int);
void schedule_SJF(Process[], int);
void schedule_PSJF(Process[], int);

#endif