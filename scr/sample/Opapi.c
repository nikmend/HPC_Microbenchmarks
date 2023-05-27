# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <errno.h>
# include <sched.h>
# include <papi.h>
# include <omp.h>

#define CPU_SETSIZE __CPU_SETSIZE
#define CPU_SET(cpu, cpusetp)   __CPU_SET (cpu, cpusetp)
#define CPU_CLR(cpu, cpusetp)   __CPU_CLR (cpu, cpusetp)
#define CPU_ISSET(cpu, cpusetp) __CPU_ISSET (cpu, cpusetp)
#define CPU_ZERO(cpusetp)       __CPU_ZERO (cpusetp)

#define MAX_EVENTS  5
#define MAX_THREADS 16

#define ERROR_RETURN(retval) { fprintf(stderr, "Error %d %s:line %d: \n", retval,__FILE__,__LINE__);  exit(retval); }


static int N_THREADS, Mode, Events[MAX_EVENTS-1];
static int Histogram[MAX_EVENTS][32];
long long  val [MAX_THREADS][MAX_EVENTS];
static int N_EVENTS, EventSet[MAX_THREADS], EventSample, SampleNumber;


void setCPU(int cpu) {
  cpu_set_t mask;

  CPU_ZERO(&mask);
  CPU_SET(cpu, &mask);
  if (sched_setaffinity( 0, sizeof(cpu_set_t), &mask) != 0)
    printf("Error: %d - %s", errno , strerror(errno)); 
}


void myOVF_handler (int Evset, void *addr, long long ovf_vect, void *context)
{
  signed long long  new [MAX_EVENTS], t;
  int  i, j, THR;
  
  PAPI_read (Evset, new);

  for (i=0; i<N_EVENTS; i++) {
    t= new[i]-val[0][i];
    val[0][i] = new[i];
    if (t<0) t = 0;
    for (j=0; j<31 && t; j++)
      t >>= 1;
    Histogram[i][j]++;
  }
}


void Sample_Start(int THR) {
  int i, j, retval;

  if (SampleNumber && THR == 0) {
    retval = PAPI_overflow(EventSet[0], EventSample, SampleNumber, 0, myOVF_handler);
    if (retval != PAPI_OK)
      ERROR_RETURN(retval);
    for (i=0; i<N_EVENTS; i++) {
      val[0][i]= 0; 
      for (j=0; j<32; j++)
        Histogram[i][j]= 0;
    }
  }
#pragma omp barrier
  PAPI_start(EventSet[THR]);
}


void Sample_Stop(int THR) {
  int retval;

  PAPI_stop(EventSet[THR], val[THR]);
  
  if (SampleNumber && THR == 0) {
    retval = PAPI_overflow(EventSet[0], EventSample, 0, 0, myOVF_handler);
    if (retval != PAPI_OK)
      ERROR_RETURN(retval);
  }
}


void Sample_Init(int argc, char *argv[]) {
  int i, j, retval;

  if (argc < 4) {
    printf("Sample parameters: NumberThreads AllocationMode SamplingMode [PAPI events ...]\n");
    exit(1);
  }
 
  N_THREADS   = (int) atof(argv[1]);
  Mode        = (int) atof(argv[2]);
  SampleNumber= (int) atof(argv[3]);

  if (!N_THREADS || N_THREADS > MAX_THREADS) {
    printf("Number of Threads is not valid\n"); 
    exit(1);
  }

  omp_set_num_threads (N_THREADS);

  if (PAPI_library_init(PAPI_VER_CURRENT)!= PAPI_VER_CURRENT) {
      printf("Library initialization error!\n");
      exit(1);
  }

  N_EVENTS = argc-4;
  for (i= 0; i<N_EVENTS; i++) {
    if ((retval = PAPI_event_name_to_code( argv[4+i], &j))!= PAPI_OK )
      ERROR_RETURN(retval);
    Events[i] = j;
  }

  if (SampleNumber) {
    if (!N_EVENTS) {
      printf("For sampling, must define an event!\n");
      exit(1);
    }
    EventSample = Events[N_EVENTS-1];
  }
}

 
int Sample_PAR_install() {
    int  i, j, THR, retval;
  
    THR = omp_get_thread_num();
    if (Mode !=-1) {
      if (!Mode)
        setCPU(THR);
      else 
        setCPU((Mode >> (3*THR)) & 7);
    }

    EventSet[THR] = PAPI_NULL;
    if ((retval= PAPI_create_eventset(&EventSet[THR])) != PAPI_OK)
      ERROR_RETURN(retval);

    if((retval = PAPI_add_event(EventSet[THR], PAPI_TOT_CYC)) != PAPI_OK)
      ERROR_RETURN(retval);

    for (i=0; i<N_EVENTS; i++) 
      if((retval = PAPI_add_event(EventSet[THR], Events[i])) != PAPI_OK)
         ERROR_RETURN(retval);

    return THR;
}


void Sample_End(void)
{
  int THR, i, j;
  for (THR=0; THR < N_THREADS; THR++) 
    {
      printf("%1.0f-", (double) THR);
      for (i=0; i<N_EVENTS+1; i++) 
        printf(": %9.0f", (double) val[THR][i]);
      printf("\n");
      if (SampleNumber && THR == 0)
        for (i=0; i<N_EVENTS; i++) {
          printf("Hist[%d]:", i);
          for (j=0; j<32; j++)
            printf(" %1.0f", (double) Histogram[i][j]);
          printf("\n");
        }
    }

  PAPI_shutdown();	
}
