# include <stdio.h>
# include <stdlib.h>
# include <omp.h>
# include <string.h>
# include <errno.h>
# include <sys/time.h>
# include <sched.h>

# define CPU_SET(cpu, cpusetp)   __CPU_SET_S (cpu, sizeof (cpu_set_t), cpusetp)
# define CPU_ZERO(cpusetp)       __CPU_ZERO_S (sizeof (cpu_set_t), cpusetp)

#define MAX_THREADS 16

static unsigned cyc_hi[MAX_THREADS], cyc_lo[MAX_THREADS];
static unsigned hi[MAX_THREADS], lo[MAX_THREADS], cnt[MAX_THREADS];

static int N_THREADS, Mode;

void setCPU(int cpu) {
  cpu_set_t mask;

  CPU_ZERO(&mask);
  CPU_SET(cpu, &mask);
  if (sched_setaffinity( 0, sizeof(cpu_set_t), &mask) != 0)
    printf("Error: %d - %s", errno , strerror(errno)); 
}


void access_counter (unsigned *hi, unsigned *lo)
{
  asm("rdtsc; movl %%edx,%0; movl %%eax,%1"
      : "=r" (*hi), "=r" (*lo)
      :
      : "%edx", "%eax");
}


void Sample_CLEAR() {
  int THR = omp_get_thread_num();
  lo[THR] = hi[THR] = cnt[THR] = 0;
}

void Sample_ON() {
int THR = omp_get_thread_num();
  access_counter(cyc_hi+THR, cyc_lo+THR);
}

void Sample_ON_Barrier() {
int THR = omp_get_thread_num();
#pragma omp barrier
  access_counter(cyc_hi+THR, cyc_lo+THR);
}


void Sample_OFF() {
  unsigned ncyc_hi, ncyc_lo;
  unsigned tlo, thi, borrow;
  int THR = omp_get_thread_num();
  access_counter(&ncyc_hi, &ncyc_lo);
  tlo = ncyc_lo - cyc_lo[THR];
  borrow = tlo > ncyc_lo;
  thi = ncyc_hi - (cyc_hi[THR] + borrow);
  lo[THR] += tlo;
  borrow = lo[THR] < tlo;
  hi[THR] += thi + borrow;
  cnt[THR]++;
}


void Sample_Init(int argc, char *argv[]) {
  int i;

  if (argc != 4) {
    printf("Sample parameters: NumberThreads AllocationMode\n");
    exit(1);
  }
 
  N_THREADS = (int) atof(argv[1]);
  Mode      = (int) atof(argv[2]);

  if (!N_THREADS || N_THREADS > MAX_THREADS) {
    printf("Number of Threads is not valid\n"); 
    exit(1);
  }

  for (i=0; i<N_THREADS; i++) {
    lo[i] = hi[i] = cnt[i] = 0;
  }

  omp_set_num_threads (N_THREADS);

#pragma omp parallel
  {
    int THR;
  
    THR = omp_get_thread_num();
    if (Mode != -1) {
      if (!Mode)
        setCPU(THR);
      else 
        setCPU((Mode >> (3*THR)) & 7);
    }
  }}

void Sample_End() { 
  int THR;
  for (THR=0; THR < N_THREADS; THR++) {
    printf(" \n%1.0f-", (double) THR);
    double time = (double) hi[THR] * (1<<30) * 4.0 + lo[THR];
    printf(":%9.0f clock cycles (%d times) \n", time, cnt[THR]);
  }
}
