#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/time.h>

asmlinkage void sys_my_gettime(unsigned long* sec, unsigned long* nsec )
{
	struct timespec now;
	getnstimeofday(&now);
	*s = now.tv_sec;
	*ns = now.tv_nsec;
}