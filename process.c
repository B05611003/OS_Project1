#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <sys/types.h>
#include "process.h"
#include "scheduler.h"
#include <sched.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/syscall.h>

int exeproc(Process proc){
	int *shared = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	//printf("%d\n",shared );
	*shared = 1;
	int pid = fork();
	
	//printf("%d\n", pid);
	//printf("%d born\n",pid);
	if (pid < 0){
		fprintf(stderr, "exeproc error\n");
		exit(1);
	}
	if (pid > 0){
		setCore(pid, CHILD_CPU);
		set_low_priority(pid);
		//printf("change shared\n");
		*shared = 0;
	}
	else if (pid == 0){ // Child Process
		//printf("%d:%d\n",*shared,getpid());
		while(*shared){
			printf("pending\n");
		}
		printf("%s:%d is now running \n",proc.name,getpid());
		//munmap(NULL, sizeof(int));
		//printf("%d is setting low\n",getpid());
		//printf("%d is here\n",getpid());
		//printf("%d\t%s\n", getpid(),proc.name);
		unsigned long startsec, startnsec;
		unsigned long finishsec, finishnsec;
		syscall(335, &startsec, &startnsec);//calculaate start time
		//printf("%d call start\n",getpid());
		for(int i = 0; i < proc.exec; i++){	//run for exec time
			volatile unsigned long j; 
			for(j = 0; j < 1000000UL; j++);	
		}
		syscall(335, &finishsec, &finishnsec);//calculaate finish time
		char message[128];
		sprintf(message, "[project1] %d %lu.%09lu %lu.%09lu\n", getpid(), startsec, startnsec, finishsec, finishnsec);
		syscall(334, message);
		//printf("Child end\n");
		exit(0);
	}

	//setCore(pid, CHILD_CPU);//???
	return pid;
}

int set_high_priority(int pid){
	struct sched_param param;
	param.sched_priority = 10;
	int ret = sched_setscheduler(pid, SCHED_FIFO, &param);
	if (ret < 0) {
		perror("sched_setscheduler error\n");
		exit(1);
	}
	return ret;
}

int set_low_priority(int pid){
	struct sched_param param;
	param.sched_priority = 1;
	int ret = sched_setscheduler(pid, SCHED_FIFO, &param);
	//printf("%d set low\n",pid);
	if (ret < 0) {

		perror("sched_setscheduler error\n");
		exit(1);
	}
	return ret;		
}
