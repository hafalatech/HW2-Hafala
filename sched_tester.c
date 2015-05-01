#include "hw2_syscalls.h"
#include <stdlib.h>
#include <stdio.h>

int fibonaci(int n)
{
        if (n < 2)
        {
                return n;
        }
        return fibonaci(n-1) + fibonaci(n-2);
}


const char* policies[] =
{
        "SCHED_OTHER", //0
        "SCHED_FIFO",//1
        "SCHED_RR",//2
        "Default", // not real, 
        "SCHED_SHORT",//4
};

const char* reasons[] =
{
        "Default",
        "A task was created",
        "A task ended",
        "A task yields the CPU",
        "An LSHORT process became overdue",
        "A previous task goes out for waiting",
        "A task with higher priority returns from waiting",
        "The time slice of the previous task has ended"
};

int main(int argc, char *argv[])
{
        char *endptr;
        int status;
        int number_of_trials;
        int num;
        int pid;
        int long_requested_time = 5000;
        int mid_requested_time = 2500;        
        int short_requested_time = 1;
        int requested_time_array[3] = {long_requested_time,mid_requested_time,short_requested_time};
        int i = 1;
        int j = 0;

        if ((argc-1) % 2 != 0)
        {
                printf ("Wrong number of arguments, got %d\n" , argc);
                return -1;
                
        }
        for (j=0 ; j<=2 ; j++) { //create 3 tests , with 3 types of requested_time
                i = 1; //reset i for new iteration
                printf("*************************************************\n");
                printf("Starting fibonaci test with requested_time = %d\n",requested_time_array[j]);
                printf("*************************************************\n");              
                while (i < argc)
                {
                        // Parse the arguments from strings to integers
                        number_of_trials = strtol(argv[i], &endptr, 10);
                        i++;
                        num = strtol(argv[i], &endptr, 10);
                        i++;
                        // Create a child process which will run the fibonaci as a SHORT
                        pid = fork();
                        if (pid > 0) {
                                struct sched_param param;
                                param.number_of_trials = number_of_trials;
                                param.requested_time = requested_time_array[j];
                                int res = sched_setscheduler(pid, SCHED_SHORT, &param);
                                if (res != 0) {
                                        printf("sched_setscheduler FAILED , got %d\n",res);
                                }
                        }
                        else {
                                sched_yield();
                                fibonaci(num);
                                _exit(0);
                        }
                }

                while (wait(&status) != -1);            // Wait for all sons to finish
                struct switch_info monitor[150];         //declare the monitor array

                for (i=0 ; i<150 ; i++) {                //reset monitor array values
                        monitor[i].time=0;
                        monitor[i].previous_pid=0;
                        monitor[i].previous_policy=0;
                        monitor[i].next_pid=0;
                        monitor[i].next_policy=0;
                        monitor[i].reason=0;
                }
                
                int result = get_scheduling_statistic(monitor);
                if (result > 150)
                        result = 150;
                printf("                Got monitor result = %d\n",result );
                printf("\n"); 
                // Print the output
                printf("|Time\t|Prev\t|PreviousPolicy\t|Next\t|Next Policy\t|Reason\n");
                for (i=0; i < result; i++)
                {
                        printf("%lu\t|%d\t|", monitor[i].time, monitor[i].previous_pid);
                        printf("%s", policies[monitor[i].previous_policy]);
                        printf("\t|%d\t|", monitor[i].next_pid);
                        printf("%s", policies[monitor[i].next_policy]);
                        printf("\t|");
                        printf("%s", reasons[monitor[i].reason]);
                        printf("\n");
                }
                printf("\n");
                printf("*************************************************\n");
                printf("Finished Test\n");
                printf("*************************************************\n");
                printf("\n");
                printf("\n");
                printf("\n");
        }
        return 0;
}
