#include <linux/linkage.h>
#include <linux/kernel.h>

asmlinkage void sys_my_print_message(char message[]){
	printk(message);
}