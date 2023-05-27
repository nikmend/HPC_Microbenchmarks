# include <stdio.h>
# include <stdlib.h>
# include <omp.h>
# include <string.h>
# include <errno.h>
# include <sys/time.h>
# include <sched.h>

#define CPU_SETSIZE __CPU_SETSIZE
#define CPU_CLR(cpu, cpusetp)   __CPU_CLR (cpu, cpusetp)
#define CPU_ISSET(cpu, cpusetp) __CPU_ISSET (cpu, cpusetp)
#define CPU_SET(cpu, cpusetp)	 __CPU_SET_S (cpu, sizeof (cpu_set_t), cpusetp)
#define CPU_ZERO(cpusetp)	 __CPU_ZERO_S (sizeof (cpu_set_t), cpusetp)


#define MAX_THREADS 16

struct timeval start[MAX_THREADS];
struct timeval stop [MAX_THREADS];

static int N_THREADS, Mode;
double INIC, FIN;

void setCPU(int cpu) {
  cpu_set_t mask;

  CPU_ZERO(&mask);
  CPU_SET(cpu, &mask);
  if (sched_setaffinity( 0, sizeof(cpu_set_t), &mask) != 0)
    printf("Error: %d - %s", errno , strerror(errno)); 
}


void Sample_Start(int THR) {
#pragma omp barrier
  gettimeofday(start + THR, (void * )0);
  INIC+=start[THR].tv_sec;
}


void Sample_Stop(int THR) {
  gettimeofday(&(stop[THR]), (void * )0);
  FIN+=stop[THR].tv_sec;
}


void Sample_Init(int argc, char *argv[]) {

  if (argc < 4) {
    printf("Sample parameters: NumberThreads AllocationMode\n");
    exit(1);
  }
 
  N_THREADS = (int) atof(argv[1]);
  Mode      = (int) atof(argv[2]);

  if (!N_THREADS || N_THREADS > MAX_THREADS) {
    printf("Number of Threads is not valid\n"); 
    exit(1);
  }

  omp_set_num_threads (N_THREADS);
}


int Sample_PAR_install() 
{
  int THR;
  
  THR = omp_get_thread_num();
  if (Mode != -1) {
    if (!Mode)
      setCPU(THR);
    else 
      setCPU((Mode >> (3*THR)) & 7);
  }

  return THR;
}

void Sample_End()
{ 
  int THR, i;

  for (THR=0; THR < N_THREADS; THR++)
  {
    printf(" \n%1.0f-", (double) THR);
    stop[THR].tv_usec -= start[THR].tv_usec;
    if (stop[THR].tv_usec < 0) {
      stop[THR].tv_usec += 1000000;
      stop[THR].tv_sec--;
    }
    stop[THR].tv_sec -= start[THR].tv_sec;

    printf(":%9.0f %9.0f\n", (double) (stop[THR].tv_sec*1000000 + stop[THR].tv_usec), INIC);
  }
}
