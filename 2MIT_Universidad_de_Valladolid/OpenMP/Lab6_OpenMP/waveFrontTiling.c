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
	#define BLOCKSIZE 2
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

	// Main section parallel with tiling

	// Dividing matrix to blocks and then perform the same approach as sequiential

	// x |x|x|x|x   |x|x|x|x
	// - --------   --------

	// x |0|0|0|d   |0|0|0|d
	// - --------   --------
	// x |0|0|d|0   |0|0|d|0
	// - --------   --------
	// x |0|d|0|0   |0|d|0|0
	// - --------   --------
	// x |d|0|0|0   |d|0|0|0

	// x |0|0|0|d   |0|0|0|d
	// - --------   --------
	// x |0|0|d|0   |0|0|d|0
	// - --------   --------
	// x |0|d|0|0   |0|d|0|0
	// - --------   --------
	// x |d|0|0|0   |d|0|0|0

	// for (z = 1; z < 2*SIZE ; z++) {
	// 	for (i=0; i<SIZE; i++) {
	// 		A[0][i] = drand48();
	// 		A[i][0] = drand48();
	// 	}		

		int newBLOCKSIZE = 1;
		// Tiling
		for (ik = 1; ik < SIZE ; ik+=BLOCKSIZE) {
			for (jk = 1; jk < SIZE; jk+=BLOCKSIZE) {					
				newBLOCKSIZE += BLOCKSIZE;
				// Fase incremiento					
				for(d=ik; d<newBLOCKSIZE+ik; d++)
					#pragma omp parallel
					#pragma omp single									
					for(e=jk; e<d-(ik-1); e++) {
						#pragma omp task
						// #ifdef DEBUG
						// 	tid = omp_get_thread_num();
						// 	printf("Thread: %d e:%d\n", tid, e);
						// #endif
						A[d-e][e] = A[d-e-1][e] + A[d-e][e-1];
						count_par++;
					}
					#pragma omp taskwait
							
				// Fase decrecimiento
				//printf("=%d %d\n", ik, jk );
				for(d=ik; d<newBLOCKSIZE; d++)
					#pragma omp parallel
					#pragma omp single
					for(e=jk; e<=(newBLOCKSIZE-d)+(ik-1); e++) {	
						#pragma omp task					
						A[newBLOCKSIZE-e][e+d-(ik-1)] = A[newBLOCKSIZE-e-1][e+d-(ik-1)] + A[newBLOCKSIZE-e][e+d-1-(ik-1)];		
						count_par++;		
						//printf("[%d,%d]\n", (newBLOCKSIZE-e), e+d-(ik-1));	
					}
					#pragma omp taskwait	

			}		
			newBLOCKSIZE = 1;	
		}

	// }

	// DEBUG init matrix test print
	#ifdef DEBUG
		for (i=0; i<SIZE; i++) {
			if (i != 0 && (i == 1 || (i%(BLOCKSIZE+1)) == 0))
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
