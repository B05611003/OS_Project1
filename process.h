#ifndef PROCESS_H
#define PROCESS_H

#define PARENT_CPU 0
#define CHILD_CPU 1 

typedef struct process{
	char name[32];
	pid_t pid;
	unsigned int ready;
	unsigned int exec;
} Process;

int exeproc(Process);
int set_low_priority(int);
int set_high_priority(int);

#endif