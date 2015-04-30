#include <linux/kernel.h>
#include <linux/sched.h>
#include "hw2_syscalls.h"

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


int sys_remaining_time(int pid)	{ /*syscall 244*/
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
		res = target->time_slice;
		if(target->is_overdue == 1){
			res = 0;
		}
	}
	return res;
}



int sys_remaining_trials(int pid) {		  /*syscall 245*/
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
		res = target->number_of_trials - target->trial_num; /* should this be +1 ??? */
		if(target->is_overdue == 1){
			res = 0;
		}
	}
	return res;
}


#define COPY_STRUCT(target, source) \
        target.time = source.time;      \
        target.previous_pid = source.previous_pid;      \
        target.previous_policy = source.previous_policy;        \
        target.next_pid = source.next_pid;      \
        target.next_policy = source.next_policy;        \
        target.reason = source.reason;

int sys_get_scheduling_statistic(struct switch_info * tasks_info) {	/*syscall 246*/
        unsigned long flags; //todo: Check if needed
        
		runqueue_t *rq;
        int index = rq->monitor_index;
        int result = rq->monitor_counter;
        int i = 0;
        if (!tasks_info) {
			return -EINVAL;
        }

        local_irq_save(flags); //todo: Check if needed
        
		rq = this_rq();
		if(result >= MONITOR_MAX_SIZE) { 
			result = MONITOR_MAX_SIZE;
 		}		

        struct switch_info monitor_copy[MONITOR_MAX_SIZE];
		for (i = index; i >= 0; i--) {
			COPY_STRUCT(monitor_copy[i], (rq->monitoringArray)[i]);
		}
		
		for (i = index; i < MONITOR_MAX_SIZE; i++) {
			COPY_STRUCT(monitor_copy[i], (rq->monitoringArray)[i]);
		}
        
		local_irq_restore(flags); //todo: Check if needed
        
		if (copy_to_user(tasks_info, monitor_copy, sizeof(struct switch_info[150]))) {
            return -EFAULT;
        }

        return result;
}







int sys_hw2_debug(int pid) {	  /*syscall 247*/
	return 0;
}

