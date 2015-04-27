#include <linux/kernel.h>
#include <linux/sched.h>
#include "syscall_maxproc.h"

int sys_set_child_max_proc(int maxp) {    //syscall 243
        if (maxp < 0)
        {
                current->child_max_proc = -1;
                return 0;
        }


        if (maxp > ((current->origin_my_max_proc) - 1) && current->origin_my_max_proc != -1)
        {
                return -EPERM;
        }

        current->child_max_proc = maxp;
        return 0;  
}


int sys_get_max_proc() {    //syscall 244
        if (current->origin_my_max_proc < 0) //if it wasnt initialized by set_child_max_proc, so its -1 by default
        {
                return -EINVAL;
        }
        return current->origin_my_max_proc;
}



int sys_get_subproc_count() {   //syscall 245
        return current->child_counter;
}




