/**
 * CPME
 * Lab5
 * Michal Ormos
 */

#include<stdio.h>
#include<stdlib.h>
#include <omp.h>

#ifdef DEBUG
	#define SIZE	8
	#define SEED	8
#else
	#define SIZE	512
	#define SEED	512
#endif

int main() {
	int i,j;

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

	// Main section
	//#pragma omp parallel for default(none) shared(A)		
	for (i=1; i<SIZE; i++) {	
		for (j=1; j<i; j++) {
		   A[i-j][j] = A[i-j-1][j] + A[i-j][j-1];
		}
	}
	for (i=SIZE-1; i>0; i--) {	
		for (j=1; j<i; j++) {
		   A[SIZE-i-j][j] = A[SIZE-i-j-1][j] + A[SIZE-i-j][j-1];
		}
	}	
	//

	{
	tiempo = omp_get_wtime() - tiempo;
	printf("Tiempo final: %lf\n", tiempo);		
	}

	// DEBUG final matrix test print
	#ifdef DEBUG
	for (i=0; i<SIZE; i++) {
		for (j=0; j<SIZE; j++) {
			printf("%.2f\t", A[i][j]);
		}
		printf("\n");
	}
		#endif

	// Check last value of matrix for test
	printf("Last num %g\n", A[SIZE-1][SIZE-1]);
}
