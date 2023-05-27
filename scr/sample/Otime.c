# include <stdio.h>
# include <stdlib.h>
# include <omp.h>
# include <string.h>
# include <errno.h>
# include <sys/time.h>


#define MAX_THREADS 100

struct timeval start;
struct timeval stop;

static int N_THREADS, Mode;


void Sample_Start() {
#pragma omp barrier
	gettimeofday(&start, (void * )0);
}


void Sample_Stop() {
	gettimeofday(&stop, (void * )0);
}


void Sample_Init(int argc, char *argv[]) {

	if (argc < 3) {
		printf("Sample parameters: NumberThreads AllocationMode\n");
		exit(1);
	}
 
	N_THREADS = (int) atof(argv[1]);

	if (!N_THREADS || N_THREADS > MAX_THREADS) {
		printf("Number of Threads is not valid\n"); 
		exit(1);
	}

	omp_set_num_threads (N_THREADS);
}


int Sample_PAR_install(){ 
	int THR;
  
	THR = omp_get_thread_num();

	return THR;
}

void Sample_End(){ 
	int THR;

	stop.tv_usec -= start.tv_usec;
	if (stop.tv_usec < 0) {
		stop.tv_usec += 1000000;
		stop.tv_sec--;
		    //printf("\n");
	}
	stop.tv_sec -= start.tv_sec;

    //printf(":%9.0f\n", (double) (stop[THR].tv_sec*1000000 + stop[THR].tv_usec));
	printf("%9.0f\n", (double) (stop.tv_sec*1000000 + stop.tv_usec));
}
