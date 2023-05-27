----- MODIFICAR -----------------

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <errno.h>
# include <papi.h>
# include <pthread.h>
# include <sched.h>
# include <linux/unistd.h>

#define CPU_SETSIZE __CPU_SETSIZE
#define CPU_SET(cpu, cpusetp)   __CPU_SET (cpu, cpusetp)
#define CPU_CLR(cpu, cpusetp)   __CPU_CLR (cpu, cpusetp)
#define CPU_ISSET(cpu, cpusetp) __CPU_ISSET (cpu, cpusetp)
#define CPU_ZERO(cpusetp)       __CPU_ZERO (cpusetp)

#define MAX_EVENTS  5
#define MAX_THREADS 16

#define ERROR_RETURN(retval) { fprintf(stderr, "Error %d %s:line %d: \n", retval,__FILE__,__LINE__);  exit(retval); }

static int Histogram[MAX_EVENTS][32];
long long  val [MAX_THREADS][MAX_EVENTS];
static int N_EVENTS, EventSet[MAX_THREADS], EventSample, SampleNumber;

pthread_mutex_t locked = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t 	cond = PTHREAD_COND_INITIALIZER;


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
  int  i, j;
  
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

  // Barrier here!!
  //pthread_mutex_lock(&locked);
  //pthread_mutex_unlock(&locked);

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


int N_THREADS, Mode, P1, P2, P3, Events[MAX_EVENTS-1];


int main(int argc, char *argv[]) {
  pthread_t THR_ARRAY[MAX_THREADS];
  int   i, j, THR, retval;
  void *p;
  
  N_THREADS = (int) atof(argv[1]);
  if (N_THREADS > MAX_THREADS)
    exit(0);
  
  if (PAPI_library_init(PAPI_VER_CURRENT)!= PAPI_VER_CURRENT) {
      printf("Library initialization error!\n");
      exit(1);
  }

  N_EVENTS = argc-6;
  for (P1= 0; P1<N_EVENTS; P1++) {
    if ((retval = PAPI_event_name_to_code( argv[6+P1], &P2))!= PAPI_OK )
      ERROR_RETURN(retval);
    Events[P1] = P2;
  }

  SampleNumber = 0;  // not enabled with pthreads to allow explicit Thread placement 
  if (SampleNumber) {
    if (!N_EVENTS) {
      printf("For sampling, must define an event!\n");
      exit(1);
    }
    EventSample = Events[N_EVENTS-1];
  }
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
  return 0;
}


void *PROC(void *args)
{
  int i, THR, retval;

  THR = (int) (long) args;

  if (!Mode)
    setCPU(THR);
  else 
    setCPU((Mode >> (3*THR)) & 7);

  EventSet[THR] = PAPI_NULL;
  if ((retval= PAPI_create_eventset(&EventSet[THR])) != PAPI_OK)
    ERROR_RETURN(retval);

  if((retval = PAPI_add_event(EventSet[THR], PAPI_TOT_CYC)) != PAPI_OK)
    ERROR_RETURN(retval);

  for (i=0; i<N_EVENTS; i++) 
    if((retval = PAPI_add_event(EventSet[THR], Events[i])) != PAPI_OK)
       ERROR_RETURN(retval);

  i = myCode(N_THREADS, THR, P1, P2, P3 );

  return (void *) (long) i;
}
