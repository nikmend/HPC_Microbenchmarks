# include <stdlib.h>
# include <stdio.h>
# include <omp.h>
# include "sample.h"

# ifndef MIN
# define MIN(x,y) ((x)<(y)?(x):(y))
# endif


float *VectorPr;
float *VectorSh;


int main (int argc, char **argv)
{
  int  	 SZ, SZ2, i;
  double v, v1;

  if (argc < 2) {
    printf("MM1c MatrixSize [Sample arguments ...]\n"); 
    return -1;
  }

  SZ  = (int) atof(argv[1]); argc--; argv++;
  SZ2 = SZ*SZ;

 Sample_Init(argc, argv);

  VectorSh = (float *) malloc (sizeof(float)*SZ2);
  for (i=0; i<SZ2; i++) {  // initialize data 
    	VectorSh[i] = 3.141516*i*i -17.2*i +4.2;
 } 

  // Allocate contiguous vectors for each thread
 VectorPr = (float *) malloc (sizeof(float) * SZ * atof(argv[1]));
  for (i=0; i<SZ; i++)  // initialize data 
    	VectorPr[i] = 3.141516*i -7*i*i +4;

#pragma omp parallel
{
  int     i, j, k, THR, al;
  double  aux;
  float   *pS, *pP;

  THR = Sample_PAR_install();

  pS  = VectorSh;
  pP  = VectorPr + SZ*THR;
  
  Sample_Start(THR);

#pragma omp for reduction(+:v1)

  for (i=1; i<= SZ; i++) {
	aux = 0.0;
	for (j=1; j<= SZ2; j++) {
		//aux += pP[i-1] * pS[j-1] + pS[SZ2%j] + pP[SZ%i];
		//aux += pS[SZ2%j] + pP[SZ%i];
		aux += pS[j%i] + pS[j-1] * pP[i-1];
		
	}
	v1+=aux;
  } 

  Sample_Stop(THR);
    
  v = v1*3; 
}
  free(VectorSh);  
  free(VectorPr);  
  Sample_End();
}
