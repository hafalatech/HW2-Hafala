#include "hw2_syscalls.h"
#include <stdio.h>
#include <assert.h>

#define HZ 512
#define MILI_TO_TICKS(x) (x)*HZ/1000
#define TICKS_TO_MILI(x) (x)/HZ*1000

const char* policies[] =
{
        "SCHED_OTHER", //0
        "SCHED_FIFO",//1
        "SCHED_RR",//2
        "Default", // not real, 
        "SCHED_SHORT",//4
        "SCHED_OVERDUE", // "SCHED_SHORT" WITH is_overdue==1
};

void print_debug(int pid)
{
        int res_debug;
        struct debug_struct* debug = malloc(sizeof( struct debug_struct));
        res_debug = hw2_debug(pid, debug);
        if(res_debug != 0){
                printf("print_debug failed\n");
                return;
        }
        
        char* policy_string;
        if (debug->policy != 4)
        {
                policy_string = policies[debug->policy];
        }
        else
        {
                if(debug->is_overdue == 0)
                {
                        policy_string = policies[4];
                }
                else
                {
                        policy_string = policies[5];
                }
        }

        printf("\n\t\t\t\t------------------DEBUG FOR PID=%d------------------\n",pid);
        

        printf("|Priority\t|Policy\t\t|requested_time(mili)\t|time_slice(mili)\t|number_of_trials\t|trial_num\t\n");
        printf("|%d\t\t|%s\t|", debug->priority, policy_string);
        printf("%d\t\t\t|%d\t\t\t|%d\t\t\t|", TICKS_TO_MILI(debug->requested_time),remaining_time(pid), debug->number_of_trials);
        printf("%d\t\t", debug->trial_num);
        printf("\n");


        printf("\t\t\t\t--------------------------------------------------------\n");
        printf("\n");
        free(debug);
        return;
}



void doLongTask()
{
        int i;
        for (i=1; i > 0; i++)
        {
                ;
        }
}

void doShortTask()
{
    short i;
    for (i=1 ; 100 > i ; i++)
    {
        ;
    }
}

void doMediumTask()
{
	int j;
	for(j=0; j< 1000 ; j++)
	{
		doShortTask();
	}
}



void testOther()
{
	int thisId = getpid();
	assert(sched_getscheduler(thisId) == 0);
	assert(remaining_time(thisId) == -1);
	assert(errno == 22);
	assert(remaining_trials(thisId) == -1);
	assert(errno == 22);
	//assert(is_SHORT(thisId) == -1);
	printf("OK\n");
}



void testBadParams()
{
    int id = fork();
    int status;
    if (id>0)
    {
            struct sched_param param;
            int expected_requested_time = 7;
            int expected_trials = 51; // more then 50 - should be invalid
            param.requested_time = expected_requested_time;
            param.number_of_trials = expected_trials;
            assert(sched_setscheduler(id, SCHED_SHORT, &param) == -1);
            assert(errno = 22);
            assert(sched_getscheduler(id) == 0);

            expected_requested_time = 5100000; // more then 5000 - should be invalid
            expected_trials = 7;
            param.requested_time = expected_requested_time;
            param.number_of_trials = expected_trials;
            assert(sched_setscheduler(id, SCHED_SHORT, &param) == -1);
            assert(errno = 22);
            assert(sched_getscheduler(id) == 0);
            wait(&status);
            printf("OK\n");
    } else if (id == 0) {
            doShortTask();
            _exit(0);
    }
}



void testSysCalls()
{
 int id = fork();
 int status;
 if (id > 0)
 {
  assert(remaining_time(id) == -1); //returns the time_slice
  assert(errno == 22);
  assert(remaining_trials(id) == -1);
  assert(errno == 22);

  struct sched_param param;
  int expected_requested_time = 4000;
  int expected_number_of_trials = 8;
  param.requested_time = expected_requested_time;
  param.number_of_trials = expected_number_of_trials;
  sched_setscheduler(id, SCHED_SHORT, &param);
  assert(is_SHORT(id) >= 0); //0 is overdue 1 is short
  int remaining_time_check = remaining_time(id); 
  int remaining_trials_check = remaining_trials(id);
  doLongTask();
  assert(remaining_time_check <= expected_requested_time);
  assert(remaining_time_check > 0);
  assert(remaining_trials_check <= expected_number_of_trials);
  assert(remaining_trials_check > 0);
  wait(&status);
  printf("OK\n");
 }
 else if (id == 0) {
  doShortTask();
  _exit(0);
 }
}


void testFork()
{
	//this test check that the father makes his son short, then the son(already short) fork a grandson,
	//which should born as short  
    int expected_requested_time = 5000;
    int id = fork();
    int status;
    if (id > 0) {
    	//father code
		struct sched_param inputParam,outputParam;
		int expected_trials = 8;
		inputParam.requested_time = expected_requested_time;
		inputParam.number_of_trials = expected_trials;
		sched_setscheduler(id, SCHED_SHORT, &inputParam); //make the son SHORT
		assert(is_SHORT(id)); //check the son became SHORT
		assert(sched_getparam(id, &outputParam) == 0);    // get the details of the son  
		assert(outputParam.requested_time  ==  MILI_TO_TICKS(expected_requested_time));
		assert(outputParam.number_of_trials <= expected_trials); //because son is making fork few lines below
		wait(&status);
		printf("OK\n");
    } else if (id == 0) {
		//son code
		struct sched_param outputParamSon;
		int remaining_time_before_fork = remaining_time(getpid());
		assert(remaining_time_before_fork == 5000); //before fork should be expected_requested_time
		assert(sched_getparam(getpid(), &outputParamSon) == 0);    // get the details of the son  
		assert(outputParamSon.requested_time  ==  MILI_TO_TICKS(expected_requested_time)); //brfore fork
		int son = fork();
		if (son == 0)
        {
			//the grandson
			int grandson_initial_time = remaining_time(getpid());
			assert(grandson_initial_time == (expected_requested_time+1)/2);

			printf("%d before long \n", remaining_time(getpid()));
			doLongTask();
			printf("%d after long \n", remaining_time(getpid()));
			assert(remaining_time(getpid()) < grandson_initial_time);
			assert(remaining_time(getpid()) > 0);
			_exit(0);
        }
        else
        {
			//the son (father of grandson) after fork
			assert(remaining_time(getpid()) == expected_requested_time/2);
			wait(&status);
        }
        _exit(0);
    }
}

void testBecomingOverdueBecauseOfTrials()					
{
    int id = fork();
    int status;
    if (id > 0) {
        struct sched_param param;
        int expected_requested_time = 60;
        int expected_trials = 2;
        param.requested_time = expected_requested_time;
        param.number_of_trials = expected_trials;
        sched_setscheduler(id, SCHED_SHORT, &param);
        assert(sched_getscheduler(id) == SCHED_SHORT);
        assert(sched_getparam(id, &param) == 0);
        assert(param.number_of_trials == expected_trials);
        wait(&status);
        printf("OK\n");
    } else if (id == 0) {
        doLongTask();
        printf(" remaining_trials is %d\n", remaining_trials(getpid()) );
        _exit(0);
    }
}


void testBecomingOverdueBecauseOfTime()					
{											//because we need to check the trial num
    int id = fork();
    int status;
    if (id > 0) {
        struct sched_param param;
        int expected_requested_time =5;
        int expected_trials = 50;
        param.requested_time = expected_requested_time;
        param.number_of_trials = expected_trials;
        sched_setscheduler(id, SCHED_SHORT, &param);


        assert(sched_getscheduler(id) == SCHED_SHORT);
        assert(sched_getparam(id, &param) == 0);
        assert(param.number_of_trials == expected_trials);
        wait(&status);
        printf("OK\n");
    } else if (id == 0) {
        int myId = getpid();
        int i = remaining_time(myId);
		doMediumTask();
        printf("masheo katan %d\n", remaining_time(myId) );
        doLongTask();  
        printf(" remaining_trials is %d\n", remaining_trials(getpid()) );      
        printf("masheo katan %d\n", remaining_time(myId) );
        assert(is_SHORT(myId) == 0 );
        _exit(0);
    }
}

void testBecomingOverdue()					
{
	int id = fork();
    int status;
    if (id > 0) {
    	//the father
        struct sched_param param;
        int expected_requested_time = 8;
        int expected_trials = 2;
        param.requested_time = expected_requested_time;
        param.number_of_trials = expected_trials;
        sched_setscheduler(id, SCHED_SHORT, &param); //MAKE THE SON SHORT
        //print_debug(getpid());


        assert(sched_getscheduler(id) == SCHED_SHORT);
        assert(sched_getparam(id, &param) == 0);
        assert(param.number_of_trials == expected_trials);

        wait(&status);
        printf("OK\n");
    } else if (id == 0) {
    	//the son
    	printf("remaining_time before %d\n", remaining_time(getpid()));
    	printf("remaining_trials before %d\n", remaining_trials(getpid()));

		assert(is_SHORT(getpid()) == 1); //0 is overdue 1 is short
		doMediumTask();
		doMediumTask();
		doMediumTask();
		doMediumTask();
		doMediumTask();
		printf("remaining_time after%d\n", remaining_time(getpid()));
		printf("remaining_trials after %d\n", remaining_trials(getpid()));
        //doLongTask();
		//assert(is_SHORT(getpid()) == 0); //0 is overdue 1 is short
        _exit(0);
    }
}

void testWTF()					
{
	int id = fork();
    int status;
    if (id > 0) {
    	printf("%d\n", getpid()); 
        struct sched_param param;
        int expected_requested_time = 5000 ;
        int expected_trials = 50;
        param.requested_time = expected_requested_time;
        param.number_of_trials = expected_trials;
        sched_setscheduler(getpid(), SCHED_SHORT, &param);
        assert(sched_getscheduler(getpid()) == SCHED_SHORT);

        wait(&status);
    } else if (id == 0) {     
        printf("%d\n", getpid());       
        assert(sched_getscheduler(getpid()) == SCHED_SHORT);
        _exit(0);
    }
}


// void testScheduleRealTimeOverLShort()
// {
// ﻿  int id = fork();
// ﻿  int status;
// ﻿  if (id > 0) {
// ﻿  ﻿  struct sched_param param1;
// ﻿  ﻿  int expected_requested_time = 30000;
// ﻿  ﻿  int expected_level = 8;
// ﻿  ﻿  param1.lshort_params.requested_time = expected_requested_time;
// ﻿  ﻿  param1.lshort_params.level = expected_level;

// ﻿  ﻿  int id2 = fork();
// ﻿  ﻿  if (id2 == 0)
// ﻿  ﻿  {
// ﻿  ﻿  ﻿  doMediumTask();
// ﻿  ﻿  ﻿  printf("\tRT son finished\n");
// ﻿  ﻿  ﻿  _exit(0);
// ﻿  ﻿  }
// ﻿  ﻿  else
// ﻿  ﻿  {
// ﻿  ﻿  ﻿  struct sched_param param2;
// ﻿  ﻿  ﻿  param2.sched_priority = 1;
// ﻿  ﻿  ﻿  sched_setscheduler(id, SCHED_LSHORT, &param1);﻿  // LSHORT process
// ﻿  ﻿  ﻿  sched_setscheduler(id2, 1, &param2);﻿  ﻿  //FIFO RealTime process
// ﻿  ﻿  }
// ﻿  ﻿  wait(&status);
// ﻿  ﻿  wait(&status);
// ﻿  ﻿  printf("OK\n");
// ﻿  } else if (id == 0) {
// ﻿  ﻿  doMediumTask();
// ﻿  ﻿  printf("\t\tLSHORT son finished\n");
// ﻿  ﻿  _exit(0);
// ﻿  }
// }

// void testScheduleRealTimeOverLShort2()
// {
// ﻿  int id = fork();
// ﻿  int status;
// ﻿  if (id > 0) {
// ﻿  ﻿  struct sched_param param1;
// ﻿  ﻿  int expected_requested_time = 30000;
// ﻿  ﻿  int expected_level = 8;
// ﻿  ﻿  param1.lshort_params.requested_time = expected_requested_time;
// ﻿  ﻿  param1.lshort_params.level = expected_level;

// ﻿  ﻿  int id2 = fork();
// ﻿  ﻿  if (id2 == 0)
// ﻿  ﻿  {
// ﻿  ﻿  ﻿  doMediumTask();
// ﻿  ﻿  ﻿  printf("\t\tLSHORT son finished\n");
// ﻿  ﻿  ﻿  _exit(0);
// ﻿  ﻿  }
// ﻿  ﻿  else
// ﻿  ﻿  {
// ﻿  ﻿  ﻿  struct sched_param param2;
// ﻿  ﻿  ﻿  param2.sched_priority = 1;
// ﻿  ﻿  ﻿  sched_setscheduler(id, 1, &param2);﻿  ﻿  ﻿  //FIFO RealTime process
// ﻿  ﻿  ﻿  sched_setscheduler(id2, SCHED_LSHORT, &param1);﻿  // LSHORT process
// ﻿  ﻿  }
// ﻿  ﻿  wait(&status);
// ﻿  ﻿  wait(&status);
// ﻿  ﻿  printf("OK\n");
// ﻿  } else if (id == 0) {
// ﻿  ﻿  doMediumTask();
// ﻿  ﻿  printf("\tRT son finished\n");
// ﻿  ﻿  _exit(0);
// ﻿  }
// }

// void testScheduleLShortOverOther()
// {
// ﻿  int id = fork();
// ﻿  int status;
// ﻿  if (id > 0) {
// ﻿  ﻿  struct sched_param param1;
// ﻿  ﻿  int expected_requested_time = 30000;
// ﻿  ﻿  int expected_level = 8;
// ﻿  ﻿  param1.lshort_params.requested_time = expected_requested_time;
// ﻿  ﻿  param1.lshort_params.level = expected_level;

// ﻿  ﻿  int id2 = fork();
// ﻿  ﻿  if (id2 == 0)
// ﻿  ﻿  {
// ﻿  ﻿  ﻿  doMediumTask();
// ﻿  ﻿  ﻿  printf("\tLSHORT son finished\n");
// ﻿  ﻿  ﻿  _exit(0);
// ﻿  ﻿  }
// ﻿  ﻿  else
// ﻿  ﻿  {
// ﻿  ﻿  ﻿  struct sched_param param2;
// ﻿  ﻿  ﻿  param2.sched_priority = 1;
// ﻿  ﻿  ﻿  sched_setscheduler(id, 0, &param2);﻿  ﻿  // regular SCHED_OTHER
// ﻿  ﻿  ﻿  sched_setscheduler(id2, SCHED_LSHORT, &param1);﻿  ﻿  // LSHORT process
// ﻿  ﻿  }
// ﻿  ﻿  wait(&status);
// ﻿  ﻿  wait(&status);
// ﻿  ﻿  printf("OK\n");
// ﻿  } else if (id == 0) {
// ﻿  ﻿  doMediumTask();
// ﻿  ﻿  printf("\t\tSCHED_OTHER son finished\n");
// ﻿  ﻿  _exit(0);
// ﻿  }
// }

// void testScheduleLShortOverOther2()
// {
// ﻿  int id = fork();
// ﻿  int status;
// ﻿  if (id > 0) {
// ﻿  ﻿  struct sched_param param1;
// ﻿  ﻿  int expected_requested_time = 30000;
// ﻿  ﻿  int expected_level = 8;
// ﻿  ﻿  param1.lshort_params.requested_time = expected_requested_time;
// ﻿  ﻿  param1.lshort_params.level = expected_level;

// ﻿  ﻿  int id2 = fork();
// ﻿  ﻿  if (id2 == 0)
// ﻿  ﻿  {
// ﻿  ﻿  ﻿  doMediumTask();
// ﻿  ﻿  ﻿  printf("\t\tSCHED_OTHER son finished\n");
// ﻿  ﻿  ﻿  _exit(0);
// ﻿  ﻿  }
// ﻿  ﻿  else
// ﻿  ﻿  {
// ﻿  ﻿  ﻿  struct sched_param param2;
// ﻿  ﻿  ﻿  param2.sched_priority = 1;
// ﻿  ﻿  ﻿  sched_setscheduler(id2, 0, &param2);﻿  ﻿  // regular SCHED_OTHER
// ﻿  ﻿  ﻿  sched_setscheduler(id, SCHED_LSHORT, &param1);﻿  ﻿  // LSHORT process
// ﻿  ﻿  }
// ﻿  ﻿  wait(&status);
// ﻿  ﻿  wait(&status);
// ﻿  ﻿  printf("OK\n");
// ﻿  } else if (id == 0) {
// ﻿  ﻿  doMediumTask();
// ﻿  ﻿  printf("\tLSHORT son finished\n");
// ﻿  ﻿  _exit(0);
// ﻿  }
// }

// void testScheduleOtherOverOverdue()
// {
// ﻿  int id = fork();
// ﻿  int status;
// ﻿  if (id > 0) {
// ﻿  ﻿  struct sched_param param1;
// ﻿  ﻿  int expected_requested_time = 1;
// ﻿  ﻿  int expected_level = 8;
// ﻿  ﻿  param1.lshort_params.requested_time = expected_requested_time;
// ﻿  ﻿  param1.lshort_params.level = expected_level;

// ﻿  ﻿  int id2 = fork();
// ﻿  ﻿  if (id2 == 0)
// ﻿  ﻿  {
// ﻿  ﻿  ﻿  doMediumTask();
// ﻿  ﻿  ﻿  printf("\t\tOVERDUE son finished\n");
// ﻿  ﻿  ﻿  _exit(0);
// ﻿  ﻿  }
// ﻿  ﻿  else
// ﻿  ﻿  {
// ﻿  ﻿  ﻿  struct sched_param param2;
// ﻿  ﻿  ﻿  param2.sched_priority = 1;
// ﻿  ﻿  ﻿  sched_setscheduler(id, 0, &param2);﻿  ﻿  // regular SCHED_OTHER
// ﻿  ﻿  ﻿  sched_setscheduler(id2, SCHED_LSHORT, &param1);﻿  ﻿  // LSHORT process
// ﻿  ﻿  }
// ﻿  ﻿  wait(&status);
// ﻿  ﻿  wait(&status);
// ﻿  ﻿  printf("OK\n");
// ﻿  } else if (id == 0) {
// ﻿  ﻿  doMediumTask();
// ﻿  ﻿  printf("\tSCHED_OTHER son finished\n");
// ﻿  ﻿  _exit(0);
// ﻿  }
// }

// void testScheduleOtherOverOverdue2()
// {
// ﻿  int id = fork();
// ﻿  int status;
// ﻿  if (id > 0) {
// ﻿  ﻿  struct sched_param param1;
// ﻿  ﻿  int expected_requested_time = 1;
// ﻿  ﻿  int expected_level = 8;
// ﻿  ﻿  param1.lshort_params.requested_time = expected_requested_time;
// ﻿  ﻿  param1.lshort_params.level = expected_level;

// ﻿  ﻿  int id2 = fork();
// ﻿  ﻿  if (id2 == 0)
// ﻿  ﻿  {
// ﻿  ﻿  ﻿  doMediumTask();
// ﻿  ﻿  ﻿  printf("\tSCHED_OTHER son finished\n");
// ﻿  ﻿  ﻿  _exit(0);
// ﻿  ﻿  }
// ﻿  ﻿  else
// ﻿  ﻿  {
// ﻿  ﻿  ﻿  struct sched_param param2;
// ﻿  ﻿  ﻿  param2.sched_priority = 1;
// ﻿  ﻿  ﻿  sched_setscheduler(id2, 0, &param2);﻿  ﻿  // regular SCHED_OTHER
// ﻿  ﻿  ﻿  sched_setscheduler(id, SCHED_LSHORT, &param1);﻿  ﻿  // LSHORT process
// ﻿  ﻿  }
// ﻿  ﻿  wait(&status);
// ﻿  ﻿  wait(&status);
// ﻿  ﻿  printf("OK\n");
// ﻿  } else if (id == 0) {
// ﻿  ﻿  doMediumTask();
// ﻿  ﻿  printf("\t\tOVERDUE son finished\n");
// ﻿  ﻿  _exit(0);
// ﻿  }
// }

// void testOverdueRoundRobin()
// {
// ﻿  int id = fork();
// ﻿  int status;
// ﻿  if (id > 0) {
// ﻿  ﻿  struct sched_param param1;
// ﻿  ﻿  int expected_requested_time = 1;
// ﻿  ﻿  int expected_level = 8;
// ﻿  ﻿  param1.lshort_params.requested_time = expected_requested_time;
// ﻿  ﻿  param1.lshort_params.level = expected_level;

// ﻿  ﻿  int id2 = fork();
// ﻿  ﻿  if (id2 == 0)
// ﻿  ﻿  {
// ﻿  ﻿  ﻿  int i, j;
// ﻿  ﻿  ﻿  for (i=0; i < 4; i++)
// ﻿  ﻿  ﻿  {
// ﻿  ﻿  ﻿  ﻿  doMediumTask();
// ﻿  ﻿  ﻿  ﻿  printf("\t\tSon2\n");
// ﻿  ﻿  ﻿  }
// ﻿  ﻿  ﻿  _exit(0);
// ﻿  ﻿  }
// ﻿  ﻿  else
// ﻿  ﻿  {
// ﻿  ﻿  ﻿  struct sched_param param2;
// ﻿  ﻿  ﻿  param2.lshort_params.requested_time = expected_requested_time;
// ﻿  ﻿  ﻿  param2.lshort_params.level = expected_level;
// ﻿  ﻿  ﻿  sched_setscheduler(id2, SCHED_LSHORT, &param1);
// ﻿  ﻿  ﻿  sched_setscheduler(id, SCHED_LSHORT, &param2);
// ﻿  ﻿  }
// ﻿  ﻿  wait(&status);
// ﻿  ﻿  wait(&status);
// ﻿  ﻿  printf("OK\n");
// ﻿  } else if (id == 0) {
// ﻿  ﻿  int i, j;
// ﻿  ﻿  for (i=0; i < 4; i++)
// ﻿  ﻿  {
// ﻿  ﻿  ﻿  doMediumTask();
// ﻿  ﻿  ﻿  printf("\tSon1\n");
// ﻿  ﻿  }
// ﻿  ﻿  _exit(0);
// ﻿  }
// }



void testMakeShort() //WILL MAKE MAIN SHORT!!
{
	 int thisId = getpid();
	 struct sched_param param;
	 int expected_requested_time = 4000;
	 int expected_number_of_trials = 35;

	 param.requested_time = expected_requested_time;
	 param.number_of_trials = expected_number_of_trials;
	 //make it short
	 sched_setscheduler(thisId, SCHED_SHORT, &param);
	 //check that it became short
	 //assert(is_SHORT(thisId) >= 0);  
	 assert(sched_getparam(thisId, &param) == 0); //get the param struct from process, 0 is Success
	 assert(sched_getscheduler(thisId) == SCHED_SHORT);

	 //check the values "thisId" have after turning it to short
 	 assert(param.number_of_trials == expected_number_of_trials);
	 assert(param.requested_time == ((expected_requested_time * HZ)/1000));
	 int i;
	 doLongTask();
	 assert(sched_getparam(thisId, &param) == 0);
	 int afterTime = remaining_time(thisId);
	 assert(afterTime > 0);
	 assert(afterTime < expected_requested_time);
	 assert(remaining_trials(thisId) > 0);
	 printf("OK\n");
}

void testMakeSonShort() //varify this test, might be invalid
{
 	int father_id = getpid();
 	struct sched_param param;
 	sched_getparam(father_id, &param); //get current params of father
 	int father_num_of_trials = remaining_trials(father_id);
 	int father_remainig_time = remaining_time(father_id); //get the time_slice
	//we are about to do a fork so lets calculate what we expect after it

	int son_expected_requested_time = param.requested_time;
	int son_expected_time_slice= ( (father_remainig_time *HZ /1000)   +1) /2;
	int son_expected_number_of_trials = (father_num_of_trials+1) /2;

	int father_expected_requested_time = param.requested_time;
	int father_expected_time_slice = (father_remainig_time *HZ /1000)  /2;
	int father_expected_number_of_trials = father_num_of_trials /2;



	int son_id = fork(); // dont forget that son_id is already SHORT from test above
	int status;
	if (son_id > 0) {
		//the father
		int new_requested_time = 5000; //fake - should not be relevant cause the process is short we cant make set on it
		int new_number_of_trials = 8; //fake - should not be relevant cause the process is short we cant make set on it

		param.requested_time = new_requested_time;
		param.number_of_trials = new_number_of_trials;

		assert(sched_getscheduler(son_id) == SCHED_SHORT); //son of a short process should be short
		assert(sched_setscheduler(son_id, SCHED_SHORT, &param) == -1);  //should fail cause its a short
		assert(sched_getscheduler(son_id) == SCHED_SHORT); //remained the same
		assert(sched_getparam(son_id, &param) == 0); //returns the struct of the son
		assert(param.requested_time == son_expected_requested_time); 

		assert(remaining_time(son_id) <= son_expected_time_slice && remaining_time(son_id)>0); 
		assert(param.number_of_trials <= son_expected_number_of_trials);

		wait(&status);
		printf("OK\n");
	} else if (son_id == 0) {
		//the son
		assert(sched_setscheduler(father_id, SCHED_SHORT, &param)== -1);  //should fail cause its a short
		assert(sched_getscheduler(father_id) == SCHED_SHORT); //remained the same (SHORT)
		assert(sched_getparam(father_id, &param) == 0);//returns the struct of the fathe

		assert(param.requested_time == father_expected_requested_time);
		assert(remaining_time(father_id) <= father_expected_time_slice && remaining_time(father_id)>0);
		assert(param.number_of_trials <= father_expected_number_of_trials);

		doShortTask();
		_exit(0);
	}
}


int main()
{
	// printf("Testing SCHED_OTHER process... ");
	// testOther();

	// printf("Testing bad parameters... ");
	// testBadParams();

	// printf("Testing new System Calls... ");
	// testSysCalls();

	// printf("Testing fork... ");
	// testFork();

	// printf("Testing testBecomingOverdueBecauseOfTrials... ");
	// testBecomingOverdueBecauseOfTrials();

	printf("Testing testBecomingOverdueBecauseOfTime... ");
	testBecomingOverdueBecauseOfTime();


	// printf("Testing becoming overdue... ");
	// testBecomingOverdue();

	// printf("Testing testWTF... ");
	// testWTF();

// ﻿  printf("Testing Overdues Round-Robin... \n");
// ﻿  testOverdueRoundRobin();

// ﻿  printf("Testing race: RT vs. LSHORT (RT is supposed to win)\n");
// ﻿  testScheduleRealTimeOverLShort();

// ﻿  printf("Testing race: RT vs. LSHORT #2 (RT is supposed to win)\n");
// ﻿  testScheduleRealTimeOverLShort2();

// ﻿  printf("Testing race: LSHORT vs. OTHER (LSHORT is supposed to win)\n");
// ﻿  testScheduleLShortOverOther();

// ﻿  printf("Testing race: LSHORT vs. OTHER #2(LSHORT is supposed to win)\n");
// ﻿  testScheduleLShortOverOther2();

// ﻿  printf("Testing race: OTHER vs. OVERDUE (OTHER is supposed to win)\n");
// ﻿  testScheduleOtherOverOverdue();

// ﻿  printf("Testing race: OTHER vs. OVERDUE #2 (OTHER is supposed to win)\n");
// ﻿  testScheduleOtherOverOverdue2();

	// printf("Testing making this process SHORT... ");
	// testMakeShort();

	// printf("Testing making son process SHORT... ");
	// testMakeSonShort();

	//printf("Success!\n");
	return 0;
}
