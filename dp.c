#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Number of philosophers
#define COUNT 5

typedef enum { thinking, hungry, eating } state;

int forks[COUNT];
state states[COUNT];

/* Sets the fork for the philosopher to 1 with race condition prevention. */
void pickup_forks(long pnum)
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int num = rand() % 3 + 1;
	char *second = num > 1 ? "seconds" : "second";

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
	printf("Philosopher %ld is eating for %d %s\n", pnum, num, second);

	// Lock resource so we don't have a race condition.
	pthread_mutex_lock(&mutex);
	forks[pnum] = 1;
	states[pnum] = eating;

	// Signal resource was modified.
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);

	sleep(num);
}

/* Sets the fork for the philosopher to 0 with race condition prevention. */ 
void return_forks(long pnum)
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;

	pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&cond, NULL);
	
	// Lock resource so we don't have a race condition.
	pthread_mutex_lock(&mutex);
	forks[pnum] = 0;
	states[pnum] = thinking;

	// Signal resource was modified.
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
}

/* Function for thread to execute. */
void *thread_exec(void* param)
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;	
	int num;
	char *second;
	int count = 0;
	long pnum = (long)param;

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	// Infinite loop to make philosopher think, hungry, and call the fork functions.
	while(count < 5)
	{
		num = rand() % 3 + 1;
	
		second = num > 1 ? "seconds" : "second";

		printf("Philosopher %ld is thinking for %d %s\n", pnum, num, second);
		sleep(num);
		printf("Philosopher %ld is hungry\n", pnum);

		// Lock resource until the philosopher is eating.
		pthread_mutex_lock(&mutex);
		pickup_forks(pnum);

		while (states[pnum] != eating)
			pthread_cond_wait(&mutex, &cond);

		pthread_mutex_unlock(&mutex);
		
		// Lock resource until the philosopher is thinking.	
		pthread_mutex_lock(&mutex);
		return_forks(pnum);

		while(states[pnum] != thinking)
			pthread_cond_wait(&mutex, &cond);

		pthread_mutex_unlock(&mutex);
		count++;
	}

	pthread_exit(NULL);
}

int main()
{
	pthread_t philosophers[COUNT];
	long i;
	int temp, rc;

	srand(time(NULL));
	
	// Initialize forks bitmap.
	for(i = 0; i < COUNT; i++)
		states[i] = forks[i] = 0;

	// Spawn threads.
	for(i = 0; i < COUNT; i++)
		pthread_create(&philosophers[i], NULL, &thread_exec, (void *)i);

	// Join threads.
	for(i = 0; i < COUNT; i++)
		pthread_join(philosophers[i], NULL);
		
	return 0;
}
