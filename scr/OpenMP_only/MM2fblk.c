# include <stdlib.h>
# include <stdio.h>
# include <omp.h>
# include "sample.h"

# ifndef MIN
# define MIN(x,y) ((x)<(y)?(x):(y))
# endif

# define DATA_SZ (1024*1024*64*3)

static double  MEM_CHUNK[DATA_SZ];

void Matrix_Init_fil(int SZ, double *a, double *b, double *c){
	int j,k;

	for (k=0; k<SZ; k++) 
		for (j=0; j<SZ; j++) {
			a[j+k*SZ] = 2.0*(j+k);  
			b[k+j*SZ] = 3.2*(j-k); // Careful! Depends on row-column order   
			c[j+k*SZ] = 0.0;
		}
}


int main (int argc, char **argv){
	int N, st, st2;

	if (argc < 4) {
		printf("MM2f MatrixSize BLK1sz BLK2sz [Sample arguments ...]\n"); 
		return -1;
	}

	N  = (int) atof(argv[1]); argc--; argv++;
	st = (int) atof(argv[1]); argc--; argv++; 
	st2= (int) atof(argv[1]); argc--; argv++;
	if (!st)  st = N; 
	if (!st2) st2 = N;

	if (N > 1024*8 || st > N || st2> N || (st & 1 != 0) || (st2 & 1 != 0) || (N & 1 != 0)){
		printf("Unvalid MatrixSize or BLK sizes \n");
		return -1;
	}

	Sample_Init(argc, argv);

#pragma omp parallel
{
	int     NTHR, THR, SZ, step, step2;
	double *a, *b, *c;
	int     i1,j1,k1,i2,j2,k2,i,j,k;

	Sample_PAR_install();
	NTHR = omp_get_num_threads();

	SZ=N;  step=st;  step2=st2;    //Local copies
	a = MEM_CHUNK;
	b = a + SZ*SZ;
	c = b + SZ*SZ;

#pragma omp master
	Matrix_Init_fil(SZ, a, b, c); 

	Sample_Start(THR);

#pragma omp for
	for (i1=0; i1<SZ; i1+=step) {
		i2=MIN(i1+step,SZ);
		for (j1=0; j1<SZ; j1+=step) {
			j2=MIN(j1+step,SZ);
			for (k1=0; k1<SZ; k1+=step2) {
				k2=MIN(k1+step2,SZ);
				for (i=i1; i<i2; i+=2)
					for (j=j1; j<j2; j+=2){
						double *pA, *pB, *pC, c0, c1, c2, c3;

						pA= a+(i*SZ+k1); 
						pB= b+(j*SZ+k1);
						pC= c+(i*SZ+j);
						k = k2-k1;
						c0= *pC;  c1= *(pC+1);  c2= *(pC+SZ); c3= *(pC+SZ+1);
       
						for (; k>0; k-=2, pA+=2, pB+=2){
							double a0, a1, a2, a3;
							double b0, b1, b2, b3;

							a0 = *pA;  a1 = *(pA+1);  a2 = *(pA+SZ);  a3 = *(pA+SZ+1);
							b0 = *pB;  b1 = *(pB+1);  b2 = *(pB+SZ);  b3 = *(pB+SZ+1);

							c0 += a0*b0 + a1*b1;
							c1 += a0*b2 + a1*b3;
							c2 += a2*b0 + a3*b1;
							c3 += a2*b2 + a3*b3;
						}
						*pC= c0;  *(pC+1)= c1;  *(pC+SZ)= c2; *(pC+SZ+1)= c3;
	}}}}
	Sample_Stop(THR);
	}

	Sample_End();
}
