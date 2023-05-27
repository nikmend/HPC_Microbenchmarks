# include <stdio.h>
# include <stdlib.h>
# include <omp.h>
# include <string.h>
# include <errno.h>
# include <sys/time.h>
# include <sched.h>

#define CPU_SETSIZE __CPU_SETSIZE
#define CPU_SET(cpu, cpusetp)   __CPU_SET (cpu, cpusetp)
#define CPU_CLR(cpu, cpusetp)   __CPU_CLR (cpu, cpusetp)
#define CPU_ISSET(cpu, cpusetp) __CPU_ISSET (cpu, cpusetp)
#define CPU_ZERO(cpusetp)       __CPU_ZERO (cpusetp)

#define MAX_THREADS 16
#define MAX_ADDR    4

struct timeval start[MAX_THREADS];
struct timeval stop [MAX_THREADS];

static int N_THREADS, Mode;

typedef struct ADDR {
  int stride1;
  long long int count1;
  int stride2;
  long int count2;
  void * lastADDR;
  void * minADDR;
  void * maxADDR;
  long long int count;
} ADDR_TYPE;

struct stride {

int delta;

int cont;

struct stride *next;

};


ADDR_TYPE ADDR_V[MAX_THREADS][MAX_ADDR];

void Sample_Addr (int ID, void * addr)
{
  int str;
  ADDR_TYPE *p;

  p = &ADDR_V[omp_get_thread_num()][ID];

  if (p->minADDR > addr)
    p->minADDR = addr;

  if (p->maxADDR < addr)
    p->maxADDR = addr;

  p->count++;

  str = labs(addr-p->lastADDR);
  p->lastADDR = addr;
  if (p->stride1 == str)
    p->count1++;
  else if (p->stride2 == str)
    p->count2++;
  else if (!p->count1)
    p->stride1 = str;
  else if (!p->count2)
    p->stride2 = str;
  else if (p->count2 >= p->count1)
    p->count2--;
  else
    p->count1--;

}

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
}


void Sample_Stop(int THR) {
  gettimeofday(&(stop[THR]), (void * )0);
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
  int THR, i;
  ADDR_TYPE *p;

  THR = omp_get_thread_num();
  if (Mode != -1) {
    if (!Mode)
      setCPU(THR);
    else
      setCPU((Mode >> (3*THR)) & 7);
  }

 for (i=0; i<MAX_ADDR; i++)
  {
    p = &ADDR_V[THR][i];
    p->lastADDR= (void *) 0;
    p->stride1 = 0;
    p->count1  = 0;
    p->stride2 = 0;
    p->count2  = 0;
    p->count   = 0;
    p->maxADDR = (void *) 0;
    p->minADDR = (void *) -1;
   }

  return THR;
}

void Sample_End()
{
  int THR, i;
  ADDR_TYPE *p;

  for (THR=0; THR < N_THREADS; THR++)
  {
    printf("%1.0f-", (double) THR);
    stop[THR].tv_usec -= start[THR].tv_usec;
    if (stop[THR].tv_usec < 0) {
      stop[THR].tv_usec += 1000000;
      stop[THR].tv_sec--;
        }
    stop[THR].tv_sec -= start[THR].tv_sec;
    printf(": %9.0f\n", (double) (stop[THR].tv_sec*1000000 + stop[THR].tv_usec));

      for (i=0; i<MAX_ADDR; i++)
      {
        p = &ADDR_V[THR][i];
        printf("%d: #= %lld, STR1= %d (%lld) STR2= %d (%ld) from %d to %d; total = %ld\n",
               	i, (p->count)/1000000, 
		(p->stride1), 
		(p->count1)/1000000, 
		(p->stride2), 
		(p->count2)/1000000, 
		p->minADDR, 
		p->maxADDR, 
		(p->maxADDR - p->minADDR)/(8*1024));
      }
    }
}
