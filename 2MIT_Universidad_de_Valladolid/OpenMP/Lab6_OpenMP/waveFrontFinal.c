/**
 * CPME
 * Lab6
 * Michal Ormos
 *
 * @TODO BugFix on fase decriemiento
 */

#include<stdio.h>
#include<stdlib.h>
#include <omp.h>

#ifdef DEBUG
	#define SIZE	5
	#define SEED	4
	#define BLOCKSIZE 5
#else
	#define SIZE	129
	#define SEED	129
	#define BLOCKSIZE 4
#endif

int main() {
	int i,j,k;
	int count_seg, count_par = 0;
	int z,ik, jk, d, e;	


	double A[ SIZE ][ SIZE ];
	double B[ SIZE ][ SIZE ];	

	srand48( SEED );

	for (i=0; i<SIZE; i++) {
		for (j=0; j<SIZE; j++) {
			A[i][j] = 0;
			B[i][j] = 0;
		}
	}	
	// 
	for (i=0; i<SIZE; i++) {
		A[0][i] = drand48();
		A[i][0] = drand48();
		B[0][i] = drand48();
		B[i][0] = drand48();		
	}

	// DEBUG init matrix test print
	#ifdef DEBUG
	for (i=0; i<SIZE; i++) {
		for (j=0; j<SIZE; j++) {
			printf("%.2f\t", A[i][j]);
		}
		printf("\n");
	}	
	#endif



	printf("============\n");
	double tiempo2 = omp_get_wtime();		

	// Main section sequiental - for comparision
	for (z = 1; z < 2*SIZE ; z++) {
		for (i=0; i<SIZE; i++) {
			B[0][i] = drand48();
			B[i][0] = drand48();
		}		

		// M[i,j] = ( M[i-1,j] + M[i,j-1] + M[i,j] ) / 3
		// Fase crecimiento
		for(d=1; d<SIZE; d++)
			for(e=1; e<d; e++) {
				B[d-e][e] = ( B[d-e-1][e] + B[d-e][e-1] + B[d-e][e] ) / 3;
				count_seg++;
			}
					
		// Fase decrecimiento
		for(d=1; d<SIZE; d++)
			for(e=1; e<=SIZE-d; e++) {
				B[SIZE-e][e+d-1] = ( B[SIZE-e-1][e+d-1] + B[SIZE-e][e+d-2] + B[SIZE-e][e+d-1] ) / 3;
				count_seg++;
			}
	}

	double tiempo_seg = omp_get_wtime() - tiempo2;
	printf("Tiempo final - segmental section %lf\n", tiempo_seg);
	printf("Count %d\n", count_seg );		
	printf("============\n");
	double tiempo;
	int iter = -1;
	int tid;
	int last;
	int u;

	printf("\n\n\n\n");

	int SIZE_MAX = (2*SIZE)+(2*SIZE-2);

	//M[i,j] = ( M[i-1,j] + M[i,j-1] + M[i,j] ) / 3
	for (z = 0; z < SIZE_MAX ; z++) {
	// 	for (i=0; i<SIZE; i++) {
	// 		B[0][i] = drand48();
	// 		B[i][0] = drand48();
	// 	}	
	// Fase crecimiento
	printf("Iter: %d\n", z);

		// for(d=0; d<=u; d++) {
			//printf("First: %d\tLast: %d\n", 0, d);
		d = z;
			for(e=0; e<=SIZE; e++) {
				tid = omp_get_thread_num();
				//A[d-e][e] = ( A[d-e-1][e] + A[d-e][e-1] + A[d-e][e] ) / 3;
				//printf("C-%d-", tid);
				printf( "U[%d,%d] ", (d-e), e);
				last = e;
			}		
			printf("\n");
		// }	
		d = z;
		// for(e=0; e<=9-z; e++) {
		// 	if ((SIZE-e) != 0 && (e+d) != 0) {			
		// 		A[SIZE-e][e+d-SIZE] = ( A[SIZE-e-1][e+d-SIZE] + A[SIZE-e][e+d-1-SIZE] + A[SIZE-e][e+d-SIZE] ) / 3;
		// 		//printf("C");					
		// 	}
		// 	printf( "L[%d,%d] ", (SIZE-e-1), (e+d-SIZE));				
		// }
		printf("\n");
	}


	// printf("\n");
	// // DEBUG init matrix test print
	// #ifdef DEBUG
	// 	for (i=0; i<SIZE; i++) {
	// 		if (i != 0 && (i == 1 || ((BLOCKSIZE+1)) == 0))
	// 			printf("\n");
	// 		for (j=0; j<SIZE; j++) {
	// 			printf("%.2f\t", A[i][j]);
	// 			if (j == 0 || (j%BLOCKSIZE) == 0)
	// 				printf("|\t");
	// 		}
	// 		printf("\n");
	// 	}
	// 	printf("\n\n\n");
	// #endif	

	printf("\n\n\n\n\nRESULTS\n");
	tiempo = omp_get_wtime() - tiempo;
	printf("Tiempo final - seguiental section %lf\n", tiempo_seg);
	printf("Count %d\n", count_seg );	
	printf("Tiempo final - parallel section: %lf\n", tiempo);
	printf("Count %d\n", count_par );	

	// DEBUG init matrix test print
	#ifdef DEBUG
	printf("\nSequiential\n");
	for (i=0; i<SIZE; i++) {
		for (j=0; j<SIZE; j++) {
			printf("%.2f\t", B[i][j]);
		}
		printf("\n");
	}			
	printf("\nParallel\n");
	for (i=0; i<SIZE; i++) {
		for (j=0; j<SIZE; j++) {
			printf("%.2f\t", A[i][j]);
		}
		printf("\n");
	}	
	#endif	

}
