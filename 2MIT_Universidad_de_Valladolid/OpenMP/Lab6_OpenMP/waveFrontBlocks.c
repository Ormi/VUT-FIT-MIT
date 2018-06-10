/**
 * CPME
 * Lab6
 * Michal Ormos
 */

#include<stdio.h>
#include<stdlib.h>
#include <omp.h>

#ifdef DEBUG
	#define SIZE	9
	#define SEED	9
	#define SIZE_COMP 8
#else
	#define SIZE	512
	#define SEED	512
#endif

int main() {
	int i,j,k;

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
	int BLOCKSIZE = 4;
	int numOfBlock = 2;
	int startBlock = 1;
	printf("%d\n",  numOfBlock);
	int ik, jk, d, e;
	int count = 0;
	// for (k = 1; k < SIZE*2; k++) {
		// for (i = 1; i < SIZE; i++) {
		// 	A[0][i] = 2;
		// 	A[i][0] = 2;
		// }

			// for (ik = 1; ik <= numOfBlock; ik++) {
			// 	for (jk = 1; jk <= numOfBlock; jk++) {
			// 		for (i = (ik-1)*BLOCKSIZE+1; i < ik*BLOCKSIZE+1; i++) {			
			// 			for (j = (jk-1)*BLOCKSIZE+1; j < jk*BLOCKSIZE+1; j++) {
			// 				A[i][j] = A[i-1][j] + A[i][j-1];
			// 				count++;
			// 				//printf("%d:%d = %f\n", i, j,A[i][j]);
			// 			}
			// 		}
			// 	}
			// }

			// // Fase crecimiento
			// for(d=1; d<SIZE; d++)
			// 	for(e=1; e<d; e++) {
			// 		A[d-e][e] = A[d-e-1][e] + A[d-e][e-1];
			// 		printf("d: %d e: %d\n",d, e );
			// 	}

			// #ifdef DEBUG
			// 	for (i=0; i<SIZE; i++) {
			// 		if (i == 5 || i == 1)
			// 			printf("\n");
			// 		for (j=0; j<SIZE; j++) {
			// 			printf("%.2f\t", A[i][j]);
			// 			if (j == 4 || j == 0)
			// 				printf("|\t");
			// 		}
			// 		printf("\n");
			// 	}
			// 	printf("\n\n\n");
			// #endif			
			

			// 1

			// Fase crecimiento
			for(d=1; d<SIZE; d++)
				for(e=1; e<d; e++) {
					A[d-e][e] = A[d-e-1][e] + A[d-e][e-1];
				}
						
			// Fase decrecimiento
			for(d=1; d<SIZE; d++)
				for(e=1; e<=SIZE-d; e++) {
					A[SIZE-e][e+d-1] = A[SIZE-e-1][e+d-1] + A[SIZE-e][e+d-2];
					//printf("d: %d e: %d\n",d, e );
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



			// Fase crecimiento
			for(d=1; d<5; d++)
				for(e=1; e<d; e++) {
					A[d-e][e] = A[d-e-1][e] + A[d-e][e-1];
				}
						
			// Fase decrecimiento
			for(d=1; d<5; d++)
				for(e=1; e<=5-d; e++) {
					A[5-e][e+d-1] = A[5-e-1][e+d-1] + A[5-e][e+d-2];
					//printf("d: %d e: %d\n",d, e );
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


			// Fase crecimiento
			for(d=5; d<SIZE; d++)
				for(e=1; e<d-BLOCKSIZE; e++) {
					A[d-e][e] = A[d-e-1][e] + A[d-e][e-1];
				}
						
			// Fase decrecimiento
			for(d=5; d<SIZE; d++)
				for(e=1; e<=5-d+BLOCKSIZE; e++) {
					A[SIZE-e][e+d-1-BLOCKSIZE] = A[SIZE-e-1][e+d-1-BLOCKSIZE] + A[SIZE-e][e+d-2-BLOCKSIZE];
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
	

			//3

			for(d=1; d<5; d++)
				for(e=5; e<d+5; e++) {
					A[d-e+BLOCKSIZE+1][e] = A[d-e+BLOCKSIZE][e] + A[d-e+BLOCKSIZE+1][e-1];
				}
						
			// Fase decrecimiento
			for(d=1; d<5; d++)
				for(e=1; e<=5-d; e++) {
					A[5-e][e+d-1+BLOCKSIZE] = A[5-e-1][e+d-1+BLOCKSIZE] + A[5-e][e+d-2+BLOCKSIZE];
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


			// 4
			for(d=5; d<SIZE; d++)
				for(e=5; e<d; e++) {
					A[d-e+BLOCKSIZE][e] = A[d-e-1+BLOCKSIZE][e] + A[d-e+BLOCKSIZE][e-1];
				}
						
			// Fase decrecimiento
			for(d=5; d<SIZE; d++)
				for(e=5; e<=SIZE-d+BLOCKSIZE; e++) {
					A[SIZE-e+BLOCKSIZE][e+d-1-BLOCKSIZE] = A[SIZE-e-1+BLOCKSIZE][e+d-1-BLOCKSIZE] + A[SIZE-e+BLOCKSIZE][e+d-2-BLOCKSIZE];
					printf(": %d : %d\n",SIZE-e+BLOCKSIZE, e+d-1-BLOCKSIZE );
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
	// }
	//


		// for (i = middle; i < SIZE; i++) {
		// 	for (j = 1; j < middle; j++) {
		// 		A[i][j] = A[i-1][j] + A[i][j-1];
		// 	}
		// 	for (j = middle; j < SIZE; j++) {
		// 		A[i][j] = A[i-1][j] + A[i][j-1];
		// 	}

		// for (i = 1; i < SIZE; i++) {
		// 	for (j = 1; j < SIZE; j++) {
		// 		A[i][j] = A[i-1][j] + A[i][j-1];
		// 	}
		// }




	{
	tiempo = omp_get_wtime() - tiempo;
	printf("Tiempo final: %lf\n", tiempo);	
	printf("Count: %d\n",count );	
	}

	// DEBUG final matrix test print
	// #ifdef DEBUG
	// for (i=0; i<SIZE; i++) {
	// 	for (j=0; j<SIZE; j++) {
	// 		printf("%.2f\t", A[i][j]);
	// 	}
	// 	printf("\n");
	// }
	// #endif

	// Check last value of matrix for test
	printf("Last num %g\n", (A[SIZE-1][SIZE-1])/1000000000000);
}
