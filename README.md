# OS-Project1 Scheduling
## abstract
Process scheduling is an essential part of an operating system supporting multiprogramming. There are various scheduling algorithms for different situations. However, many of them are based on the same concept: priority-driven scheduling. For priority-driven scheduling, it is important to assign an appropriate priority level to each process. The assignment depends on the desired scheduling policy. For example, a round-robin scheduler can be implemented by assigning the same priority level to all the processes. In this project, we would like to study the relation between priority assignment and scheduling policies.
## design
### The project is bulit in three differrent c code :main,scheduler,process，which contain specific function describe below:
* **`main`:** read input data and choose the assigned scheduling algorithm and call the corresponding scheduling function in schedule.c
* **`scheduler`:** a function that can set cpu affinity to processes that run for the scheduling test, functions which run the process of scheduling, there are four scheduling function which can control when will the process been activated or terminated by changing scheduling priority parameters:  
`void schedule_FCFS(Process[], int);`  
`void schedule_RR(Process[], int);`  
`void schedule_SJF(Process[], int);`  
`void schedule_PSJF(Process[], int);`  
* **`process`:** function to run forked child process and priority changing
### extra kernel syscall:
* **`my_gettime`:** using gettimeofday() to get system timestamps  
* **`my_print`:** prink() the messages  
### flowchart:
![](https://i.imgur.com/aU8b7u8.png)



## 2.kernel version
* kernel 4.15.8(HW1的版本)
## 3.usage 
`git clone https://github.com/B05611003/OS_Project1`  
`cd OS_Project1`  
`make`  
`sh run.sh` #you might need sudo to run the program correctly


