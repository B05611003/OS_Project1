#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <sys/types.h>
#include "process.h"
#include "scheduler.h"
#include <sched.h>
#include <sys/syscall.h>

int exeproc(Process proc){
	int pid = fork();
	if (pid < 0){
		fprintf(stderr, "exeproc error\n");
		exit(1);
	}
	if (pid == 0){ // Child Process
		printf("Child start\n");
		unsigned long startsec, startnsec;
		unsigned long finishsec, finishnsec;
		syscall(333, &startsec, &startnsec);//calculaate start time
		for(int i = 0; i < proc.exec; i++){	//run for exec time
			volatile unsigned long j; 
			for(j = 0; j < 1000000UL; j++);	
		}
		syscall(333, &finishsec, &finishnsec);//calculaate finish time
		char message[128];
		sprintf(message, "[project1] %d %lu.%09lu %lu.%09lu\n", getpid(), startsec, startnsec, finishsec, finishnsec);
		syscall(334, message);
		printf("Child end\n");
		exit(0);
	}
	setCore(pid, CHILD_CPU);//???
	return pid;
}

int set_high_priority(int pid){
	struct sched_param param;
	param.sched_priority = 0;
	int ret = sched_setscheduler(pid, SCHED_OTHER, &param);
	if (ret < 0) {
		perror("sched_setscheduler error\n");
		exit(1);
	}
	return ret;
}

int set_low_priority(int pid){
	struct sched_param param;
	param.sched_priority = 0;
	int ret = sched_setscheduler(pid, SCHED_IDLE, &param);
	
	if (ret < 0) {

		perror("sched_setscheduler error\n");
		exit(1);
	}
	return ret;		
}
