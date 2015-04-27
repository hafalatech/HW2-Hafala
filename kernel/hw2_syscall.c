#include <linux/kernel.h>
#include <linux/sched.h>
#include "hw2_syscall.h"

int sys_is_SHORT(int pid) {    			/*syscall 243*/
	//Checking if pid is neg
	if (pid < 0) {
		return -EINVAL;
    }
	//getting process by pid
	struct task_struct *target;
    if (pid == 0) {
        target = &init_task;
    } else {
        target = find_task_by_pid(pid);
    }
    //Checking if process exists
	if (!target) {
        return -EINVAL;
    }
	int res = -EINVAL;
	//Returning 0 in case process is OVERDUE or 1 if process is SHORT
	if(target->policy == SCHED_SHORT) {
		if(target->is_overdue == 1){
			res = 0;
		}
		res = 1;
	}
	return res;
}


int sys_remaining_time(int pid)	    	/*syscall 244*/
{
	//Checking if pid is neg
	if (pid < 0) {
		return -EINVAL;
    }
	//getting process by pid
	struct task_struct *target;
    if (pid == 0) {
        target = &init_task;
    } else {
        target = find_task_by_pid(pid);
    }
    //Checking if process exists
	if (!target) {
        return -EINVAL;
    }
	int res = -EINVAL;
	//Returning 0 in case process is OVERDUE or remaining_time if process is SHORT
	if(target->policy == SCHED_SHORT) {
		res = target->requested_time - target->remaining_time;
		if(target->is_overdue == 1){
			res = 0;
		}
	}
	return res;
}



int sys_remaining_trials(int pid) 		  /*syscall 245*/
{
    //Checking if pid is neg
	if (pid < 0) {
		return -EINVAL;
    }
	//getting process by pid
	struct task_struct *target;
    if (pid == 0) {
        target = &init_task;
    } else {
        target = find_task_by_pid(pid);
    }
    //Checking if process exists
	if (!target) {
        return -EINVAL;
    }
	int res = -EINVAL;
	//Returning 0 in case process is OVERDUE or remaining_trials if process is SHORT
	if(target->policy == SCHED_SHORT) {
		res = target->trial_num - target->remaining_time;
		if(target->is_overdue == 1){
			res = 0;
		}
	}
	return res;
}



int sys_debug(int pid)	    			/*syscall 246*/
{
	
}

