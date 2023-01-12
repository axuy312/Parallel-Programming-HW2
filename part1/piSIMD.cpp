//ref: https://github.com/stephanfr/Xoshiro256PlusSIMD

#define __AVX2_AVAILABLE__

#include "./include/SIMDInstructionSet.h"

#include "./include/Xoshiro256Plus.h"
#include "Xoshiro256PlusReference.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

typedef SEFUtility::RNG::Xoshiro256Plus<SIMDInstructionSet::AVX2> Xoshiro256PlusAVX2;

long long int *number_of_thread_tosses;

void * tosses(void *arg) {
	int idxThread = *((int*)arg) + 1;
	long long int number_in_circle = 0;
	long long int number_of_tosses = number_of_thread_tosses[0];
	
	Xoshiro256PlusAVX2 avx_rng(time(NULL));
	
	for ( long long int toss = 0; toss < number_of_tosses; toss+=2) {
		
		auto next4_avx = avx_rng.dnext4(-1.0, 1.0);
		
		double x = next4_avx[0];
		double y = next4_avx[1];
		
		double distance_squared = x * x + y * y;
		if ( distance_squared <= 1)
			number_in_circle++;
		
		x = next4_avx[2];
		y = next4_avx[3];
		
		distance_squared = x * x + y * y;
		if ( distance_squared <= 1)
			number_in_circle++;
	}
	number_of_thread_tosses[idxThread] = number_in_circle;
	
    return NULL;
}

int main(int argc, char *argv[])
{
   	srand(time(NULL));
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
	
	Xoshiro256PlusAVX2 avx_rng(time(NULL));
	//計算剩餘的tosses
	for (int idx = 0; idx < number_of_tosses%number_of_threads; idx+=2)
	{
		auto next4_avx = avx_rng.dnext4(-1.0, 1.0);
		
		double x = next4_avx[0];
		double y = next4_avx[1];
		
		double distance_squared = x * x + y * y;
		if ( distance_squared <= 1)
			number_in_circle++;
		
		x = next4_avx[2];
		y = next4_avx[3];
		
		distance_squared = x * x + y * y;
		if ( distance_squared <= 1)
			number_in_circle++;
	}
	
	for (int idx = 0; idx < number_of_threads; idx++)
	{
		pthread_join(threads[idx], NULL);
		number_in_circle += number_of_thread_tosses[idx + 1];
	}
	
	//double pi_estimate = 4 * number_in_circle /(( double ) number_of_tosses);
	double pi_estimate = 4.0 * (( double ) number_in_circle) / (( double ) number_of_tosses);
	printf("%lf\n", pi_estimate);
	exit(0);
}
