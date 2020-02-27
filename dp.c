#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Number of philosophers
#define COUNT 5

int chopsticks[COUNT];
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* Makes the philosopher pick up their chopsticks and eat for a random amount of time. */
void eat(long pnum)
{
	int num = rand() % 5 + 1;

	printf("Philosopher %ld is hungry\n", pnum);

	pthread_mutex_lock(&mutex);

	// Wait for both chopsticks to be available.
	if(pnum == 5)
		while(chopsticks[pnum] == 1 || chopsticks[0] == 1)	
			pthread_cond_wait(&cond, &mutex);
	else
		while(chopsticks[pnum] == 1 || chopsticks[pnum + 1] == 1)
			pthread_cond_wait(&cond, &mutex);

	// Change state of philosopher's chopsticks.
	chopsticks[pnum] = 1;	
	if(pnum == 5)
		chopsticks[0] = 1;
	else
		chopsticks[pnum + 1] = 1;

	// Signal resource was modified.
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
	
	printf("Philosopher %ld is eating for %d %s\n", pnum, num, num > 1 ? "seconds" : "second");
	sleep(num);
}

/* Makes the philosophers return the chopsticks and think for a random amount of time. */ 
void think(long pnum)
{
	int num = rand() % 5 + 1;
	
	// Lock resource so we don't have a race condition.
	pthread_mutex_lock(&mutex);
	chopsticks[pnum] = 0;
	if(pnum == 5)
		chopsticks[0] = 0;
	else
		chopsticks[pnum + 1] = 0;

	// Signal resource was modified.
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);

	printf("Philosopher %ld is thinking for %d %s\n", pnum, num, num > 1 ? "seconds" : "second");
	sleep(num);
}

/* Function for thread to execute. */
void *thread_exec(void* param)
{
	long pnum = (long)param;

	// Infinite loop to make philosohers eat and think.
	while(1)
	{
		eat(pnum);
		think(pnum);
	}
}

int main()
{
	pthread_t philosophers[COUNT];
	long i;

	srand(time(NULL));
	
	// Initialize chopsticks to the unused state.
	for(i = 0; i < COUNT; i++)
		chopsticks[i] = 0;

	// Spawn threads.
	for(i = 0; i < COUNT; i++)
		pthread_create(&philosophers[i], NULL, &thread_exec, (void *)i);

	// Join threads.
	for(i = 0; i < COUNT; i++)
		pthread_join(philosophers[i], NULL);
		
	return 0;
}
