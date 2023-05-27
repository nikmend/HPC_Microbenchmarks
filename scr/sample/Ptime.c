--- MODIFICAR ----


# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <string.h>
# include <sched.h>
# include <errno.h>
# include <linux/unistd.h>

#define CPU_SETSIZE __CPU_SETSIZE
#define CPU_SET(cpu, cpusetp)   __CPU_SET (cpu, cpusetp)
#define CPU_CLR(cpu, cpusetp)   __CPU_CLR (cpu, cpusetp)
#define CPU_ISSET(cpu, cpusetp) __CPU_ISSET (cpu, cpusetp)
#define CPU_ZERO(cpusetp)       __CPU_ZERO (cpusetp)

#define MAX_THREADS 16

int N_THREADS, Mode;

struct timeval start[MAX_THREADS];
struct timeval stop [MAX_THREADS];

void setCPU(int cpu) {
  cpu_set_t mask;

  CPU_ZERO(&mask);
  CPU_SET(cpu, &mask);
  if (sched_setaffinity( 0, sizeof(cpu_set_t), &mask) != 0)
    printf("Error: %d - %s", errno , strerror(errno)); 
}

void Sample_Start(int THR) {
  //pthread_mutex_lock(&locked);
  //pthread_mutex_unlock(&locked);
  gettimeofday(start + THR, (void * )0);
}


void Sample_Stop(int THR) {
  gettimeofday(&(stop[THR]), (void * )0);
}


int Sample_Init (int argc, char *argv[]) {
  int       i, THR;
  pthread_t THR_ARRAY[MAX_THREADS];
  void      *p;
  
  N_THREADS = (int) atof(argv[1]);

  if (N_THREADS > MAX_THREADS)
    exit(0);
 
  Mode= (int) atof(argv[2]);
  P1  = (int) atof(argv[3]);
  P2  = (int) atof(argv[4]);
  P3  = (int) atof(argv[5]);
 
  for (THR=0; THR<N_THREADS; THR++)
    pthread_create (THR_ARRAY+THR, NULL , PROC, (void *) (long) THR);

  for (THR=0; THR<N_THREADS; THR++) {
    pthread_join (THR_ARRAY[THR], &p);
    i = (int) *(long *) p;
    printf("%1.0f-:(%9.0f)", (double) THR, (double) i);
    stop[THR].tv_usec -= start[THR].tv_usec;
    if (stop[THR].tv_usec < 0) {
      stop[THR].tv_usec += 1000000;
      stop[THR].tv_sec--;
    }
    stop[THR].tv_sec -= start[THR].tv_sec;

    printf(": %9.0f\n", (double) (stop[THR].tv_sec*1000000 + stop[THR].tv_usec));
  }

  return 0;
}


void *PROC(void *args)
{
  int i, THR;

  THR = (int) (long) args;

  if (!Mode)
    setCPU(THR);
  else 
    setCPU((Mode >> (3*THR)) & 7);

  i = myCode(N_THREADS, THR, P1, P2, P3 );
  return (void *) (long) i;
}
