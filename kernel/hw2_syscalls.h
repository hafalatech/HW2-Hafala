#include <asm/errno.h>
extern int errno;

/*== ************************* [STRUCTS] ************************* ==*/
struct switch_info {
    int previous_pid;
    int next_pid;
    int previous_policy;
    int next_policy;
    unsigned long time;
    int reason;
};
/*== ************************************************************* ==*/


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



int hw2_debug(int pid)					/*246 - for debug using*/
{

}
