# include <stdlib.h>
# include <stdio.h>
# include <omp.h>
# include "sample.h"

# ifndef MIN
# define MIN(x,y) ((x)<(y)?(x):(y))
# endif


float *VectorP;
float *VectorS;

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

  // Allocate same vectors for each thread
 VectorS = (float *) malloc (sizeof(float) * SZ2);
  for (i=0; i<SZ2; i++)   // initialize data 
        VectorS[i] = 3.141516*i + 7 + 4*i*i;

  // Allocate contiguous vectors for each thread
 VectorP = (float *) malloc (sizeof(float) * SZ2);
  for (i=0; i<SZ2; i++)   // initialize data 
    	VectorP[i] = 3.141516*i +7*i*i +4;

#pragma omp parallel
{
  int     i, j, k, THR, al; 
  double  aux;	
  float   *pP, *pS;
	pP = VectorP;
	pS = VectorS;

  THR = Sample_PAR_install();

  
  Sample_Start(THR);

#pragma omp for reduction(+:v1)
	for (i=0; i< 1000; i++) {
		aux = 0.0;
		for (j=0; j<SZ2; j++) 
			aux += pS[j] * pS[i]; 
//printf("%d %d pP[i]=[%7.0f] dir=[%d] pP[i*j]=[%7.0f] dir=[%d] %f v1=%7.0f\n",i, j, pP[i],&pP[i],pP[i*j],&pP[i*j], aux,v1);
//	}
		v1+=aux;
	}
  Sample_Stop(THR);
    
  v = v1*3; 
}
  free(VectorP);  
  free(VectorS);  
  Sample_End();
}
