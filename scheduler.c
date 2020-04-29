#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sysinfo.h>
#include "scheduler.h"
#include "process.h"
#include <errno.h> 


void setCore(pid_t pid, int core) {
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(core, &mask);
	
	if(sched_setaffinity(pid, sizeof(mask), &mask) < 0) {
		perror("setaffinity error");	
		exit(1);
	}
}

int comparator(const void *a, const void *b) {
	if (((Process*)a)->ready == ((Process*)b)->ready)	return ((Process*)a)->exec - ((Process*)b)->exec;
	return ((Process*)a)->ready - ((Process*)b)->ready;
}




int comparator_fcfs(const void *a, const void *b) {
	return ((Process*)a)->ready - ((Process*)b)->ready;
}

void schedule_FCFS(Process *proc, int num_procs){
	int running = -1;
	int curr = 0;
	int remaining = num_procs;
	int ingproc=0;
	int next = -1;
	//to make sure the child running fairly,set the parent to other CPU

	if (get_nprocs()>1){
		//printf("%d\n", get_nprocs());
		setCore(getpid(), PARENT_CPU);
	}
	struct sched_param param;
	param.sched_priority = 0;
	sched_setscheduler(getpid(), SCHED_OTHER, &param);
	
	
	qsort(proc, num_procs, sizeof(Process), comparator_fcfs);

	while(1) {
		if (running != -1 && proc[running].exec == 0){//runtime end
			waitpid(proc[running].pid, NULL, 0);
			remaining--;
			if(remaining == 0) break;
			running = -1;
		}
		
		for (int i = ingproc; i < num_procs; i++) {//for every proc
			if (proc[i].ready == curr) {// if proc had come
				proc[i].pid = exeproc(proc[i]);//init process
				printf("at %d, %s %d is fork()\n",curr,proc[i].name,proc[i].pid);
				fflush(stdout);
				ingproc++;
			}
			else{
				break;
			}
		}
		next = -1;
		if (running != -1)	next = running; // FIFO is non-preemtive
		else{
			for (int i = num_procs-remaining; i < ingproc; i++) {
				if(proc[i].exec > 0){
					next = i;
					break;	
				}
			}
		}
		if (next != running && next != -1) {
			//fprintf(stderr, "Context switch\n");
			//printf("next=%d\n",proc[next].pid);
			set_high_priority(proc[next].pid);
			set_low_priority(proc[running].pid);
			printf("at %d,running %s\n", curr,proc[next].name);
			fflush(stdout);
			running = next;

		}
		/* Run 1 unit time */
		volatile unsigned long i;
		for(i = 0; i < 1000000UL; i++);

		/* Executing time - 1 */
		if (running != -1)
			proc[running].exec--;
		curr++;
	}
	printf("------------------------------\n");
	printf("name\tid\n");
	for(int i = 0; i < num_procs;i++){
		printf("%s\t%d\n",proc[i].name,proc[i].pid);
	}
}

void schedule_RR(Process *proc, int num_procs){
	int running = -1;
	int curr = 0;
	int event = 0;
	int remaining = num_procs;
	int ingproc=0;
	int next = -1;
	int front = -1, rear = -1;
	int items[20];
	//to make sure the child running fairly,set the parent to other CPU

	if (get_nprocs()>1){
		//printf("%d\n", get_nprocs());
		setCore(getpid(), PARENT_CPU);
	}
	struct sched_param param;
	param.sched_priority = 0;
	sched_setscheduler(getpid(), SCHED_OTHER, &param);
	
	qsort(proc, num_procs, sizeof(Process), comparator_fcfs);

	while(1) {
		if (running != -1 && proc[running].exec == 0){//runtime end
			waitpid(proc[running].pid, NULL, 0);
			running = -1;
			remaining--;
			if(remaining == 0) break;
		}
		for (int i = ingproc; i < num_procs; i++) {	//for every proc
			if (proc[i].ready == curr) {			//if proc had come
				proc[i].pid = exeproc(proc[i]);		//init process
				printf("at %d, %s %d is fork()\n",curr,proc[i].name,proc[i].pid);
				fflush(stdout);
				if (front == -1) front = 0;
				rear = (rear + 1) % 20;
				items[rear] = i;
				ingproc++;
			}
			else{
				break;
			}
		}
		next = -1;
		if (running == -1) {	//currrently not running program
			//printf("here:%d\n",curr );
			if (front != -1){
				next = items[front];
				if (front == rear) {
					front = -1;
					rear = -1;
				}
				else {
					front = (front + 1) % 20;
				} 
			}
		}
		else if ((curr-event) % 500 == 0) {// time up and change
			if (proc[running].exec>0){
				if (front == -1) front = 0;
				rear = (rear + 1) % 20;
				items[rear] = running;
			}
			if (front == -1)	continue;
			else{
				next = items[front];
				if (front == rear) {
					front = -1;
					rear = -1;
				}
				else {
					front = (front + 1) % 20;
				} 
			}
		} 
		else {
			next = running;
		}

		
		if (next != running && next != -1) {
			//fprintf(stderr, "Context switch\n");
			set_high_priority(proc[next].pid);
			set_low_priority(proc[running].pid);
			printf("at %d,running %s\n", curr,proc[next].name);
			fflush(stdout);
			running = next;
			event = curr;
		}
		/* Run 1 unit time */
		volatile unsigned long i;
		for(i = 0; i < 1000000UL; i++);

		/* Executing time - 1 */
		if (running != -1)
			proc[running].exec--;
		curr++;
	}
	printf("------------------------------\n");
	printf("name\tid\n");
	for(int i = 0; i < num_procs;i++){
		printf("%s\t%d\n",proc[i].name,proc[i].pid);
	}
}

void schedule_SJF(Process *proc, int num_procs){
	int running = -1;
	int curr = 0;
	int remaining = num_procs;
	int ingproc=0;
	int next = -1;
	//to make sure the child running fairly,set the parent to other CPU
	if (get_nprocs()>1){
		//printf("%d\n", get_nprocs());
		setCore(getpid(), PARENT_CPU);
	}
	struct sched_param param;
	param.sched_priority = 0;
	sched_setscheduler(getpid(), SCHED_OTHER, &param);
	qsort(proc, num_procs, sizeof(Process), comparator_fcfs);

	while(1) {
		if (running != -1 && proc[running].exec <= 0){//runtime end
			waitpid(proc[running].pid, NULL, 0);
			running = -1;
			remaining--;
			if(remaining == 0) break;
		}
		
		for (int i = ingproc; i < num_procs; i++) {//for every proc
			if (proc[i].ready == curr) {// if proc had come
				proc[i].pid = exeproc(proc[i]);//init process
				printf("at %d, %s %d is fork()\n",curr,proc[i].name,proc[i].pid);
				fflush(stdout);

				//set_low_priority(proc[i].pid);//cooldown it		
				ingproc++;
			}
			else{
				break;
			}
		}
		next = -1;
		if (running != -1) 
			next = running;//non -preemtive
		else{
			for (int i = 0; i < ingproc; i++) {
				if ((proc[i].exec != 0) && (next == -1 || proc[i].exec < proc[next].exec))
					next = i;	
				}
		} 
		
		if (next != running && next != -1) {
			//fprintf(stderr, "Context switch\n");
			set_high_priority(proc[next].pid);
			set_low_priority(proc[running].pid);
			printf("at %d,running %s\n", curr,proc[next].name);
			fflush(stdout);
			running = next;
			
		}
		/* Run 1 unit time */
		volatile unsigned long i;
		for(i = 0; i < 1000000UL; i++);

		/* Executing time - 1 */
		if (running != -1)
			proc[running].exec--;
		curr++;
	}
	printf("------------------------------\n");
	printf("name\tid\n");
	for(int i = 0; i < num_procs;i++){
		printf("%s\t%d\n",proc[i].name,proc[i].pid);
	}
}

void schedule_PSJF(Process *proc, int num_procs){
	int running = -1;
	int curr = 0;
	int remaining = num_procs;
	int ingproc=0;
	int next = -1;
	//to make sure the child running fairly,set the parent to other CPU
	if (get_nprocs()>1){
		//printf("%d\n", get_nprocs());
		setCore(getpid(), PARENT_CPU);
	}
	struct sched_param param;
	param.sched_priority = 0;
	sched_setscheduler(getpid(), SCHED_OTHER, &param);
	
	qsort(proc, num_procs, sizeof(Process), comparator);

	while(1) {
		if (running != -1 && proc[running].exec == 0){//runtime end
			waitpid(proc[running].pid, NULL, 0);
			running = -1;
			remaining--;
			if(remaining == 0) break;
		}
		
		for (int i = ingproc; i < num_procs; i++) {//for every proc
			if (proc[i].ready == curr) {// if proc had come
				proc[i].pid = exeproc(proc[i]);//init process
				printf("at %d,%s %d is fork()\n",curr,proc[i].name,proc[i].pid);
				fflush(stdout);

				//set_low_priority(proc[i].pid);//cooldown it		
				ingproc++;
			}
			else{
				break;
			}
		}
		next = -1;
		for (int i = 0; i < ingproc; i++) {
			if (proc[i].exec != 0 && (next == -1 || proc[i].exec < proc[next].exec))
				next = i;	
		}
		
		if (next != running && next != -1) {
			//fprintf(stderr, "Context switch\n");
			set_high_priority(proc[next].pid);
			set_low_priority(proc[running].pid);
			printf("at %d,running %s\n", curr,proc[next].name);
			fflush(stdout);
			running = next;	
		}
		/* Run 1 unit time */
		volatile unsigned long i;
		for(i = 0; i < 1000000UL; i++);

		/* Executing time - 1 */
		if (running != -1)
			proc[running].exec--;
		curr++;
	}
	printf("------------------------------\n");
	printf("name\tid\n");
	for(int i = 0; i < num_procs;i++){
		printf("%s\t%d\n",proc[i].name,proc[i].pid);
	}
}