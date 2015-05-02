#include "hw2_syscalls.h"
#include <stdio.h>
#include <assert.h>

#define HZ 512

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
	for(j=0; 1000 > j; j++)
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

void testMakeShort()
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
	 assert(sched_getscheduler(thisId) == SCHED_SHORT);
	 assert(sched_getparam(thisId, &param) == 0); //get the param struct from process, 0 is Success

	 //check the values "thisId" have after turning it to short
	 assert(param.requested_time == expected_requested_time);
	 assert(param.number_of_trials == expected_number_of_trials);


	 int i;
	 doMediumTask();
	 assert(sched_getparam(thisId, &param) == 0);
	 int afterTime = remaining_time(thisId);
	 assert(afterTime > 0);
	 assert(afterTime < expected_requested_time);
	 assert(remaining_trials(thisId) > 0);
	 printf("OK\n");
}

void testMakeSonShort()
 {
	int id = fork();
	int status;
	if (id > 0) {
	struct sched_param param;
	int expected_requested_time = 5000;
	int expected_number_of_trials = 8;
	param.requested_time = expected_requested_time;
	param.number_of_trials = expected_number_of_trials;
	sched_setscheduler(id, SCHED_SHORT, &param);
	assert(sched_getscheduler(id) == SCHED_SHORT);
	assert(sched_getparam(id, &param) == 0);
	assert(param.requested_time == expected_requested_time);
	assert(param.number_of_trials == expected_number_of_trials);
	wait(&status);
	printf("OK\n");
	} else if (id == 0) {
		//assert(is_SHORT(id) >= 0);
		doShortTask();
		_exit(0);
	}
}

void testBadParams()
{
	int id = fork();
	int status;
	if (id>0)
	{
		struct sched_param param;
		int expected_requested_time = 6000;
		int expected_number_of_trials = 6;
		param.requested_time = expected_requested_time;
		param.number_of_trials = expected_number_of_trials;
		assert(sched_setscheduler(id, SCHED_SHORT, &param) == -1);
		assert(errno = 22);
		assert(sched_getscheduler(id) == 0); //sched_getscheduler returns the policy, so we expect 0 cause no policy was set


		expected_requested_time = 4000;
		expected_number_of_trials = 200;
		param.requested_time = expected_requested_time;
		param.number_of_trials = expected_number_of_trials;
		assert(sched_setscheduler(id, SCHED_SHORT, &param) == -1);
		assert(errno = 22);
		assert(sched_getscheduler(id) == 0); //sched_getscheduler returns the policy, so we expect 0 cause no policy was set
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
		int remaining_time_check = remaining_time(id); 
		int remaining_trials_check = remaining_trials(id);
		assert(remaining_time_check <= expected_requested_time);
		assert(remaining_time_check > 0);
		printf("%d\n", remaining_trials_check);
		assert(remaining_trials_check == 0);   // ???????
		wait(&status);
		printf("OK\n");
	}
	else if (id == 0) {
		//assert(is_SHORT(id) >= 0);
		doShortTask();
		_exit(0);
	}
}

// void testFork()
// {
// 	int expected_requested_time = 200;
// 	int id = fork();
// 	int status;
// 	if (id > 0) {
// 		struct sched_param param;
// 		int expected_number_of_trials = 8;
// 		param.requested_time = expected_requested_time;
// 		param.number_of_trials = expected_number_of_trials;
// 		sched_setscheduler(id, SCHED_SHORT, &param);
// 		assert(is_SHORT(id) >= 0);
// 		assert(sched_getscheduler(id) == SCHED_SHORT);
// 		assert(sched_getparam(id, &param) == 0);
// 		assert(param.requested_time == expected_requested_time);
// 		assert(param.number_of_trials == expected_number_of_trials);
// 		wait(&status);
// 		printf("OK\n");
// 	} else if (id == 0) {
// 		assert(remaining_time(getpid()) == expected_requested_time);
// 		int son = fork();	
// 		if (son == 0)
// 		{
// 			int grandson_initial_time = remaining_time(getpid());
// 			assert(grandson_initial_time < (expected_requested_time*2)/3); //?????????
// 			assert(grandson_initial_time > 0);
// 			doMediumTask();
// 			assert(remaining_time(getpid()) < grandson_initial_time);
// 			_exit(0);
// 		}
// 		else
// 		{
// 			assert(remaining_time(getpid()) < expected_requested_time/2);
// 			wait(&status);
// 		}
// 		_exit(0);
// 	}
// }

// void testBecomingOverdue()
// {
// 	int id = fork();
// 	int status;
// 	if (id > 0) {
// 	struct sched_param param;
// 	int expected_requested_time = 2;
// 	int expected_level = 8;
// 	param.lshort_params.requested_time = expected_requested_time;
// 	param.lshort_params.level = expected_level;
// 	sched_setscheduler(id, SCHED_LSHORT, &param);
// 	assert(sched_getscheduler(id) == SCHED_LSHORT);
// 	assert(sched_getparam(id, &param) == 0);
// 	assert(param.lshort_params.requested_time == expected_requested_time);
// 	assert(param.lshort_params.level == expected_level);
// 	wait(&status);
// 	printf("OK\n");
// 	} else if (id == 0) {
// 		int myId = getpid();
// 		int i = lshort_query_overdue_time(myId);
// 		for (i; i < 2; )
// 		{
// 			i = lshort_query_overdue_time(myId);
// 			doShortTask();
// 		}
// 		_exit(0);
// 	}
// }

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

int main()
{
	printf("Testing SCHED_OTHER process... ");
	testOther();

	printf("Testing making this process SHORT... ");
	testMakeShort();

	printf("Testing making son process SHORT... ");
	testMakeSonShort();

	printf("Testing bad parameters... ");
	testBadParams();

	printf("Testing new System Calls... ");
	testSysCalls();

// ﻿  printf("Testing becoming overdue... ");
// ﻿  testBecomingOverdue();

	// printf("Testing fork... ");
	// testFork();

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

	//printf("Success!\n");
	return 0;
}
