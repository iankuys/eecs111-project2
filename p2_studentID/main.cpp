#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <pthread.h>
#include "types_p2.h"
#include "p2_threads.h"
#include "utils.h"

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct timeval t_global_start;

Restroom restroom;

int main(int argc, char **argv)
{
	// This is to set the global start time
	gettimeofday(&t_global_start, NULL);

	pthread_t tid = 0;
	int status = 0;
	int work = 0;

	if ((argc != 2) || (atoi(argv[1]) <= 0))
	{
		printf("[ERROR] Expecting 1 integer argument with value greater than 0, but got none.\n");
		printf("[USAGE] ./p2_exec <number>\n");
		exit(1);
	}

	const int n_people = 2 * atoi(argv[1]);
	int n_male = atoi(argv[1]);
	int n_female = atoi(argv[1]);
	struct timeval t_curr;

	pthread_t people[n_people];
	srand(time(0));

	for (int i = 0; i < n_people; i++)
	{
		int gender = rand() % 2;
		if (((gender == 0) && (n_male != 0) || (n_female == 0)))
		{
			gettimeofday(&t_curr, NULL);
			printf("[%lu ms][Input] A person (Man) goes into the queue.\n", get_elasped_time(t_global_start, t_curr));
			if (pthread_create(&people[i], NULL, maleThread, NULL))
			{
				fprintf(stderr, "Error creating thread");
			}
			n_male--;
		}
		else
		{
			gettimeofday(&t_curr, NULL);
			printf("[%lu ms][Input] A person (Woman) goes into the queue.\n", get_elasped_time(t_global_start, t_curr));
			if (pthread_create(&people[i], NULL, femaleThread, NULL))
			{
				fprintf(stderr, "Error creating thread");
			}
			n_female--;
		}
		int interval = rand() % 5 + 1;
		usleep(MSEC(interval));
	}

	for (int j = 0; j < n_people; j++)
	{
		if (pthread_join(people[j], NULL))
		{
			fprintf(stderr, "Error joining thread");
		}
	}

	return 0;
}
