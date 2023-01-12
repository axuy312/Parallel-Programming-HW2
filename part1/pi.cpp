#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <cmath>
#include <unistd.h>

const int MAX = std::sqrt(RAND_MAX);
long long int *number_of_thread_tosses;

void * tosses(void *arg) {
	int idxThread = *((int*)arg) + 1;
	long long int number_in_circle = 0;
	long long int number_of_tosses = number_of_thread_tosses[0];
	
	unsigned int seed = idxThread;//time(NULL);
	
	
	
	for ( long long int toss = 0; toss < number_of_tosses; toss++) {
		//double x = (((double)rand_r(&seed)) / MAX - 0.5) * 2.000002;
		int r = rand_r(&seed);
		double x = (double(r % MAX) / MAX - 0.5);
		double y = (double(r / MAX) / MAX - 0.5);
		
		double distance_squared = x * x + y * y;
		if ( distance_squared <= 0.250000)
			number_in_circle++;
	}
	number_of_thread_tosses[idxThread] = number_in_circle;
	
    return NULL;
}

int main(int argc, char *argv[])
{
	int number_of_threads = atoi(argv[1]);
	long long int number_of_tosses = atoll(argv[2]);
	long long int number_in_circle = 0;
	//Create thread's tmpSum and number of each thread for loop
	number_of_thread_tosses = new long long int[number_of_threads+1]();
	number_of_thread_tosses[0] = number_of_tosses / (long long int)number_of_threads;
	
	pthread_t *threads = new pthread_t[number_of_threads];
	for (int idx = 0; idx < number_of_threads; idx++)
	{
		int *idxThread = new int();
		*idxThread = idx;
		if (pthread_create(&(threads[idx]), NULL, tosses, idxThread) != 0) {
			printf("Error: pthread_create\n");
		}	
	}
	
	unsigned int seed = 0;//time(NULL);
	//計算剩餘的tosses
	for (int idx = 0; idx < number_of_tosses%number_of_threads; idx++)
	{
		//double x = (((double)rand_r(&seed)) / MAX - 0.5) * 2.000002;
		int r = rand_r(&seed);
		double x = (double(r % MAX) / MAX - 0.5);
		double y = (double(r / MAX) / MAX - 0.5);
		
		double distance_squared = x * x + y * y;
		if ( distance_squared <= 0.2500000)
			number_in_circle++;
	}
	
	for (int idx = 0; idx < number_of_threads; idx++)
	{
		pthread_join(threads[idx], NULL);
		number_in_circle += number_of_thread_tosses[idx + 1];
	}
	
	//double pi_estimate = 4 * number_in_circle /(( double ) number_of_tosses);
	double pi_estimate = std::floor(4.0 * (( double ) number_in_circle) / (( double ) number_of_tosses) * 1000.0)/1000.0;
	printf("%.3lf\n", pi_estimate);
	exit(0);
}
