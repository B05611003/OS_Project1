#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
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
	setCore(getpid(), PARENT_CPU);
	set_high_priority(getpid());
	
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
				set_low_priority(proc[i].pid);//cooldown it		
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
			set_high_priority(proc[next].pid);
			set_low_priority(proc[running].pid);
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
}

void schedule_RR(Process *proc, int num_procs){
	int running = -1;
	int curr = 0;
	int event = 0;
	int remaining = num_procs;
	int ingproc=0;
	int next = -1;
	//to make sure the child running fairly,set the parent to other CPU
	setCore(getpid(), PARENT_CPU);
	set_high_priority(getpid());
	
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
				set_low_priority(proc[i].pid);		//cooldown it		
				ingproc++;
			}
			else{
				break;
			}
		}
		next = -1;
		if (running == -1) {	//currrently not running program
			for (int i = 0; i < ingproc; i++) 
				if (proc[i].exec > 0) {
					next = i;
					break;
			}
		}
		else if ((curr-event) % 500 == 0) {// time up and change
			next = (running+1) % num_procs;
			while (proc[next].pid == -1 || proc[next].exec == 0) 
				next = (next+1) % num_procs;
		} 
		else {
			next = running;
		}

		
		if (next != running && next != -1) {
			//fprintf(stderr, "Context switch\n");
			set_high_priority(proc[next].pid);
			set_low_priority(proc[running].pid);
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
}

void schedule_SJF(Process *proc, int num_procs){
	int running = -1;
	int curr = 0;
	int remaining = num_procs;
	int ingproc=0;
	int next = -1;
	//to make sure the child running fairly,set the parent to other CPU
	setCore(getpid(), PARENT_CPU);
	set_high_priority(getpid());
	
	qsort(proc, num_procs, sizeof(Process), comparator);

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
				set_low_priority(proc[i].pid);//cooldown it		
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
			for (int i = num_procs-remaining; i < ingproc; i++) {
				if ((proc[i].exec != 0) && (next == -1 || proc[i].exec < proc[next].exec))
					next = i;	
				}
		} 
		
		if (next != running && next != -1) {
			//fprintf(stderr, "Context switch\n");
			set_high_priority(proc[next].pid);
			set_low_priority(proc[running].pid);
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
}

void schedule_PSJF(Process *proc, int num_procs){
	int running = -1;
	int curr = 0;
	int remaining = num_procs;
	int ingproc=0;
	int next = -1;
	//to make sure the child running fairly,set the parent to other CPU
	setCore(getpid(), PARENT_CPU);
	set_high_priority(getpid());
	
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
				set_low_priority(proc[i].pid);//cooldown it		
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
}