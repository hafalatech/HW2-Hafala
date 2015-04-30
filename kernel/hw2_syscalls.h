#include <asm/errno.h>
extern int errno;

#define SCHED_SHORT    3               /* HW2 - Roy for sched_tester.c*/

int is_SHORT(int pid)				/*243*/
{
	unsigned int res;
    __asm__
    (
                    "int $0x80;"
                    : "=a" (res)
                    : "0" (243) ,"b" (pid)
                    : "memory"
    );
    if (res>=(unsigned long)(-125))
    {
            errno = -res;
            res = -1;
    }
    return (int) res;
}



int remaining_time(int pid) 		/*244*/
{
	unsigned int res;
    __asm__
    (
                    "int $0x80;"
                    : "=a" (res)
                    : "0" (244) ,"b" (pid)
                    : "memory"
    );
    if (res>=(unsigned long)(-125))
    {
            errno = -res;
            res = -1;
    }
    return (int) res;
}



int remaining_trials(int pid)		/*245*/
{
    unsigned int res;
    __asm__
    (
                    "int $0x80;"
                    : "=a" (res)
                    : "0" (245) ,"b" (pid)
                    : "memory"
    );
    if (res>=(unsigned long)(-125))
    {
            errno = -res;
            res = -1;
    }
    return (int) res;
}



int get_scheduling_statistic(struct switch_info * tasks_info)   /*246*/
{
    unsigned int res;
    __asm__
    (
                    "int $0x80;"
                    : "=a" (res)
                    : "0" (246) ,"b" (tasks_info)
                    : "memory"
    );
    if (res>=(unsigned long)(-125))
    {
		errno = -res;
		res = -1;
    }
    return (int) res;
}




int hw2_debug(int pid)					/*247 - for debug using*/
{
    unsigned int res;
    __asm__
    (
                    "int $0x80;"
                    : "=a" (res)
                    : "0" (247) ,"b" (pid)
                    : "memory"
    );
    return 0;
}
