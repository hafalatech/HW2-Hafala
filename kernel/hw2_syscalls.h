#include <asm/errno.h>
extern int errno;


/*== ************************* [DEFINES] ************************* ==*/
#define SCHED_SHORT﻿ 4  ﻿ 
/*== ************************************************** ==*/



/*== ************************* [STRUCTS] ************************* ==*/
struct switch_info {
    int previous_pid;
    int next_pid;
    int previous_policy;
    int next_policy;
    unsigned long time;
    int reason;
};

struct short_sched_param {
   int requested_time;
   int trial_num;
};

struct sched_param {﻿
﻿  union {
﻿  ﻿  int sched_priority;
﻿  ﻿  struct short_sched_param short_params;
﻿  };
};
/*== ************************************************** ==*/



/************************************************
 * ﻿  ﻿  ﻿  ﻿  wrapper functions
 ***********************************************/
int lshort_query_remaining_time(int pid)
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
/************************************************/
int lshort_query_overdue_time(int pid)
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
/************************************************/
int get_scheduling_statistic(struct switch_info * tasks_info)﻿  /*HW2-yoav*/
{
    unsigned int res;
    __asm__
    (
                    "int $0x80;"
                    : "=a" (res)
                    : "0" (245) ,"b" (tasks_info)
                    : "memory"
    );
    if (res>=(unsigned long)(-125))
    {
            errno = -res;
            res = -1;
    }
    return (int) res;
}
