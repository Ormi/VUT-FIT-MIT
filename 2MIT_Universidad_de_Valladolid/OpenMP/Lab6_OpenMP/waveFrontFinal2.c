/**
 * CPME
 * Matrix operations
 * M[i,j] = ( M[i-1,j] + M[i,j-1] + M[i,j] ) / 3
 * with
 *	OpenMP Paralelism
 *	Macro-pipeline
 *	Double Buffer
 *
 * @date Mar 2018
 * @author Michal Ormos
 * copyright UVa
 *
 *
 * compilation (with or without parallel function)
 *	gcc -fopenmp waveFrontFinal.c
 *	or
 *	gcc waveFrontFinal.c -lgomp
 */

#include<stdio.h>
#include<stdlib.h>
#include <omp.h>

#ifdef DEBUG
	#define SIZE	4
	#define SEED	4
#else
	#define SIZE	512
	#define SEED	512
#endif

int main() {
	// Matrix declarations
	int i,j;

	// Tesk id
	int tid;

	// Operations coutners for test purpose
	unsigned long count_seg, count_par;
	count_par = count_seg = 0;

	// Time counters for tests purspose
	double tiempo, tiempo2, tiempo_seg, tiempo_par;

	// Main test sequiental control variables
	int e,d;

	// Main parallel control variables
	int z, a, b, MAX, start;


	// First matrix for parallel computing
	double A[ SIZE ][ SIZE ];
	// Second matrix for parallel computing
	double B[ SIZE ][ SIZE ];
	// Matrix for sequiential test computing
	double C[ SIZE ][ SIZE ];

	srand48( SEED );

	/*******************************/
	/*-------INICIALIZATIONS-------*/
	/*******************************/
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
		B[0][i] = B[0][i];
		B[i][0] = B[i][0];	
		#ifdef DEBUG
			C[0][i] = drand48();
			C[i][0] = drand48();
		#endif	
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

	/*******************************/
	/*-------SEQUIENTAL PART-------*/
	/*******************************/
	tiempo2 = omp_get_wtime();		

	// Main section sequiental - for comparision
	for (z = 1; z < 2*SIZE ; z++) {
		for (i=0; i<SIZE; i++) {
			C[0][i] = drand48();
			C[i][0] = drand48();
		}		

		// M[i,j] = ( M[i-1,j] + M[i,j-1] + M[i,j] ) / 3
		// Fase crecimiento
		for(d=1; d<SIZE; d++)
			for(e=1; e<d; e++) {
				C[d-e][e] = ( C[d-e-1][e] + C[d-e][e-1] + C[d-e][e] ) / 3;
				count_seg++;
			}
					
		// Fase decrecimiento
		for(d=1; d<SIZE; d++)
			for(e=1; e<=SIZE-d; e++) {
				C[SIZE-e][e+d-1] = ( C[SIZE-e-1][e+d-1] + C[SIZE-e][e+d-2] + C[SIZE-e][e+d-1] ) / 3;
				count_seg++;
			}
	}
	tiempo_seg = omp_get_wtime() - tiempo2;	

	/*******************************/
	/*--------PARALLEL PART--------*/
	/*******************************/
	tiempo = omp_get_wtime();		

	// SIZE is from 5x5 but from [0,4]=5 elements
	int SIZE_MAX = (2*(SIZE+1))+(2*(SIZE+1)-2);

	//M[i,j] = ( M[i-1,j] + M[i,j-1] + M[i,j] ) / 3
	for (z = 0; z < SIZE_MAX ; z++) {
		for (i=0; i<SIZE; i++) {
			A[0][i] = drand48();
			A[i][0] = drand48();
		}		

	#ifdef DEBUG
		printf("Iter: %d\n", z);
	#endif

	//#pragma omp parallel private(a,b,z, start, MAX) shared(A,B)	
	#pragma omp parallel
	#pragma omp single		
		{	

			//FIRST HALF
			if (z >= SIZE)
				MAX = SIZE;
			else
				MAX = z;
			if (z >= 2*SIZE+1)
				start = z - (2*SIZE+1);
			else 
				start = 0;


			#pragma omp collapse(2)
			#pragma omp task				
			for (a = start; a <= MAX; a++) {	
				for (b = 0; b <= a; b++) {
					#ifdef DEBUG
						printf("[%d,%d]", a-b, b );						
					#endif
					if ((a-b) != 0 && b != 0) {		
						if ( z % 2 == 0 ) 	
							A[a-b][b] = (B[a-b-1][b] + B[a-b][b-1] + B[a-b][b]) / 3;
						else
							B[a-b][b] = (A[a-b-1][b] + A[a-b][b-1] + A[a-b][b]) / 3;
						#ifdef DEBUG
							tid = omp_get_thread_num(); printf("|TID:%d|",tid);
						#endif
						count_par++;
					}
				}
				#ifdef DEBUG
					printf("\n");
				#endif
			}
			#pragma omp taskwait

			//MAIN anti-diagonal


			//SECOND HALF
			if (z >= SIZE) {
				if (z >= (4*SIZE)-1)
					start = z-(3*SIZE+1);
				else 
					start = 1;	
				//#pragma omp for schedule(static,2) 
				#pragma omp collapse(2)
				#pragma omp task				
				for (a = start; a < z-SIZE+1; a++) {
					for (b = 0; b<(SIZE+1)-a; b++) {
						#ifdef DEBUG
							printf("[%d,%d]", SIZE-b, a+b );
						#endif
						if ((SIZE-b) != 0 && (a+b) !=0) {
							if ( z % 2 != 0)
								A[SIZE-b][a+b] = (B[SIZE-b-1][a+b] + B[SIZE-b][a+b-1] + B[SIZE-b][a+b]) / 3;
							else 
								B[SIZE-b][a+b] = (A[SIZE-b-1][a+b] + A[SIZE-b][a+b-1] + A[SIZE-b][a+b]) / 3;
							#ifdef DEBUG
								tid = omp_get_thread_num(); printf("|TID:%d|",tid);
							#endif							
							count_par++;
						}
					}
					#ifdef DEBUG
						printf("\n");	
					#endif
				}
				#pragma omp taskwait
			}		
		}				
	
	}
	tiempo_par = omp_get_wtime() - tiempo;

	/*******************************/
	/*-----------RESULTS-----------*/
	/*******************************/
	printf("RESULTS\n=======\n");
	printf("Tiempo final - seguiental section: %lf\n", tiempo_seg);
	printf("Count %lu\n", count_seg );
	printf("Tiempo final - parallel section: %lf\n", tiempo_par);
	printf("Count %lu\n", count_par );	

	// DEBUG init matrix test print
	#ifdef DEBUG
	printf("\nParallel A\n");
	for (i=0; i<SIZE; i++) {
		for (j=0; j<SIZE; j++) {
			printf("%.2f\t", A[i][j]);
		}
		printf("\n");
	}			
	printf("\nParallel B\n");
	for (i=0; i<SIZE; i++) {
		for (j=0; j<SIZE; j++) {
			printf("%.2f\t", B[i][j]);
		}
		printf("\n");
	}	
	printf("\nSequential C\n");
	for (i=0; i<SIZE; i++) {
		for (j=0; j<SIZE; j++) {
			printf("%.2f\t", C[i][j]);
		}
		printf("\n");
	}

	printf("END\n");	
	#endif	

}
