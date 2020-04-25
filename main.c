#define _GNU_SOURCE

#include <string.h>
#include <unistd.h>
#include "process.h"
#include "scheduler.h"


Policy checkPolicy(char *policy_name) {
	if (!strcmp(policy_name, "FIFO")) {
		return FIFO;	
	} else if (!strcmp(policy_name, "RR")) {
		return RR;
	} else if (!strcmp(policy_name, "SJF")) {
		return SJF;	
	} else if (!strcmp(policy_name, "PSJF")) {
		return PSJF;	
	} else {
		fprintf(stderr, "Schedule Policy error!\n");
		exit(1);
	}
} 

int main(int argc, char **argv){
	char policy_name[4];//policy name
	int process_num;//process number
	scanf("%s", policy_name);
	scanf("%d",&process_num);

	Policy policy = checkPolicy(policy_name);//check the policy type
	Process *proc = (Process*)malloc(sizeof(Process)*process_num);//create process array
	//input process 
	for (int i = 0; i < process_num; i++) {	
		scanf("%s %u %u", proc[i].name, &proc[i].ready, &proc[i].exec);
		proc[i].pid = -1;
	}
	//run secduler
	if(policy == FCFS){
		schedule_FCFS(proc, process_num);
	}
	else if (policy == RR){
		schedule_RR(proc, process_num);
	}
	else if (policy == SJF){
		schedule_SJF(proc, process_num);
	}
	else if (policy == PSJF){
		schedule_PSJF(proc, process_num);
	}
	return 0;
}