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
	#define SIZE	9
	#define SEED	9
	#define BLOCKSIZE 9
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

		// Fase crecimiento
		for(d=1; d<SIZE; d++)
			for(e=1; e<d; e++) {
				B[d-e][e] = B[d-e-1][e] + B[d-e][e-1];
				count_seg++;
			}
					
		// Fase decrecimiento
		for(d=1; d<SIZE; d++)
			for(e=1; e<=SIZE-d; e++) {
				B[SIZE-e][e+d-1] = B[SIZE-e-1][e+d-1] + B[SIZE-e][e+d-2];
				count_seg++;
			}
	}

	double tiempo_seg = omp_get_wtime() - tiempo2;
	printf("Tiempo final - segmental section %lf\n", tiempo_seg);
	printf("Count %d\n", count_seg );		
	printf("============\n");
	double tiempo = omp_get_wtime();	
	int tid;	
	int diagonal_num = 0;
	int first, last = 0;

	// Fase crecimiento
	for(d=1; d<=SIZE; d++) {
		//printf("First:\t[%d,%d]\t", (d-e), e );
		for(e=1; e<d; e++) {
			if (first == 0)
				printf("First [%d,%d]\t", (d-e), e); first = 1;
			A[d-e][e] = A[d-e-1][e] + A[d-e][e-1];
			count_par++;
			last = e;
		}
		first = 0;
		printf("Last [%d,%d]\t", (d-last), last);		
		//printf("Last:\t[%d,%d]\n", (d-e), e );
		//printf("Time:\t%d\t first:\t%d\t last:\t%d\n", diagonal_num, 0,e);
		printf("Diagonal %d\t Value %lf\n", diagonal_num, A[d-last][last]);
		diagonal_num++;
	}

	printf("Main anti-diagonal\n");
	// Fase decrecimiento
	for(d=1; d<SIZE; d++) {
		for(e=1; e<SIZE-d; e++) {
			if (first == 0)
				printf("First [%d,%d]\t", (SIZE-e-1), (e+d)); first = 1;			
			A[SIZE-e][e+d] = A[SIZE-e-1][e+d] + A[SIZE-e][e+d-1];
			count_par++;
			last = e;
		}
		first = 0;
		printf("Last [%d,%d]\t", (SIZE-last), (last+d));				
		//printf("Time:\t%d\t first:\t%d\t last:\t%d\n", diagonal_num, d, e+d);
		printf("Diagonal %d\t Value %lf\n", diagonal_num, A[SIZE-last][last+d] );
		diagonal_num++;		
	}

	printf("\n");
	// DEBUG init matrix test print
	#ifdef DEBUG
		for (i=0; i<SIZE; i++) {
			if (i != 0 && (i == 1 || ((BLOCKSIZE+1)) == 0))
				printf("\n");
			for (j=0; j<SIZE; j++) {
				printf("%.2f\t", A[i][j]);
				if (j == 0 || (j%BLOCKSIZE) == 0)
					printf("|\t");
			}
			printf("\n");
		}
		printf("\n\n\n");
	#endif	

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
