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
	#define SIZE	4
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
	double C[ SIZE ][ SIZE ];

	srand48( SEED );

	for (i=0; i<SIZE; i++) {
		for (j=0; j<SIZE; j++) {
			A[i][j] = 0;
			B[i][j] = 0;
			C[i][j] = 0;			
		}
	}	
	// 
	for (i=0; i<SIZE; i++) {
		A[0][i] = drand48();
		A[i][0] = drand48();
		B[0][i] = drand48();
		B[i][0] = drand48();	
		C[0][i] = A[0][i];
		C[i][0] = A[i][0];	
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
	int a,b;
	int MAX;
	int start;

	printf("\n\n\n\n");

	// SIZE is from 5x5 but from [0,4]=5 elements
	int SIZE_MAX = (2*(SIZE+1))+(2*(SIZE+1)-2);

	//M[i,j] = ( M[i-1,j] + M[i,j-1] + M[i,j] ) / 3
	for (z = 0; z < SIZE_MAX ; z++) {

	printf("Iter: %d\n", z);

		//FIRST HALF
		// if (z >= SIZE)
		// 	MAX = SIZE;
		// else
		// 	MAX = z;
		// if (z >= 2*SIZE+1)
		// 	start = z - (2*SIZE+1);
		// else 
		// 	start = 0;

		if (z < 5)
			a = z;
		else
			a = 5;
		// for (a = start; a <= MAX; a++) {
			printf("U ");			
			for (b = 0; b <= a; b++) {
				printf("[%d,%d]", a-b, b );						
				if ((a-b) != 0 && b != 0) {			
					A[a-b][b] = (A[a-b-1][b] + A[a-b][b-1] + A[a-b][b]) / 3;
				}
			}
			printf("\n");
		// }

		//MAIN anti-diagonal


		//SECOND HALF
		if (z >= SIZE) {
			if (z >= (4*SIZE)-1)
				start = z-(3*SIZE+1);
			else 
				start = 1;			
			for (a = start; a < z-SIZE+1; a++) {
				//printf("L ");
				for (b = 0; b<(SIZE+1)-a; b++) {
					printf("[%d,%d]", SIZE-b, a+b );
					if ((SIZE-b) != 0 && (a+b) !=0) {
						A[SIZE-b][a+b] = (A[SIZE-b-1][a+b] + A[SIZE-b][a+b-1] + A[SIZE-b][a+b]) / 3;					
					}
				}
				printf("\n");	
			}			
		}	
	
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
	printf("\nA\n");
	for (i=0; i<SIZE; i++) {
		for (j=0; j<SIZE; j++) {
			printf("%.2f\t", A[i][j]);
		}
		printf("\n");
	}			
	printf("\nB\n");
	for (i=0; i<SIZE; i++) {
		for (j=0; j<SIZE; j++) {
			printf("%.2f\t", B[i][j]);
		}
		printf("\n");
	}	
	printf("\nC\n");
	for (i=0; i<SIZE; i++) {
		for (j=0; j<SIZE; j++) {
			printf("%.2f\t", C[i][j]);
		}
		printf("\n");
	}		
	#endif	

}
