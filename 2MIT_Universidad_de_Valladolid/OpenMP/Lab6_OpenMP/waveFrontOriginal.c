/**
 * CPME
 * Lab6
 * Michal Ormos
 */

#include<stdio.h>
#include<stdlib.h>
#include <omp.h>

#ifdef DEBUG
	#define SIZE	3
	#define SEED	9
#else
	#define SIZE	2049
	#define SEED	513
#endif

int main() {
	int i,j,k,d,e;
	int count = 0;

	double A[ SIZE ][ SIZE ];

	srand48( SEED );

	for (i=0; i<SIZE; i++) {
		for (j=0; j<SIZE; j++) {
			A[i][j] = 0;
		}
	}	
	// 
	for (i=0; i<SIZE; i++) {
		A[0][i] = 2;
		A[i][0] = 2;
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
	double tiempo = omp_get_wtime();		


	// Fase crecimiento
	for(d=1; d<SIZE; d++)
		for(e=1; e<d; e++) {
			A[d-e][e] = A[d-e-1][e] + A[d-e][e-1];
			count++;
		}
				
	// Fase decrecimiento
	for(d=1; d<SIZE; d++)
		for(e=1; e<=SIZE-d; e++) {
			A[SIZE-e][e+d-1] = A[SIZE-e-1][e+d-1] + A[SIZE-e][e+d-2];
			//printf("d: %d e: %d\n",d, e );
			count++;
		}

	#ifdef DEBUG
		for (i=0; i<SIZE; i++) {
			if (i == 5 || i == 1)
				printf("\n");
			for (j=0; j<SIZE; j++) {
				printf("%.2f\t", A[i][j]);
				if (j == 4 || j == 0)
					printf("|\t");
			}
			printf("\n");
		}
		printf("\n\n\n");
	#endif	


	tiempo = omp_get_wtime() - tiempo;
	printf("Tiempo final: %lf\n", tiempo);	
	printf("Count %d\n", count );

	// DEBUG init matrix test print
	#ifdef DEBUG
	for (i=0; i<SIZE; i++) {
		for (j=0; j<SIZE; j++) {
			printf("%.2f\t", A[i][j]);
		}
		printf("\n");
	}	
	#endif	

}
