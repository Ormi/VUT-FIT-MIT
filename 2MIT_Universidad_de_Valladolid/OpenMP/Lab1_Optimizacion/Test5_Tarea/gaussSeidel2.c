/*
 * Asignatura: Computación Paralela y Modelos Emergentes
 * Máster en Ingeniería Informática, Universidad de Valladolid
 *
 * Triangularizar matriz por eliminación Gaussiana
 * Ejemplo de código secuencial a optimizar
 *
 * (c) Arturo Gonzalez-Escribano, 2018
 */
#include<stdio.h>
#include<stdlib.h>

#ifdef DEBUG
	#define SIZE	10
#else
	#define SIZE	2879
#endif

#define SEED	6834723
#define BLOCK	2879

int main() {
	int i,j,k;

	double A[ SIZE ][ SIZE ];

	/* Inicialización pseudo-aleatoria con semilla */
	srand48( SEED );
	for (i=0; i<SIZE; i++) 
		for (j=0; j<SIZE; j++) 
			A[i][j] = drand48();

#ifdef DEBUG
	/* NO ALTERAR: Codigo para depuracion */
	for (i=0; i<SIZE; i++) {
		for (j=0; j<SIZE; j++) {
			printf("%8.4lf ", A[i][j] );
		}
		printf( "\n" );
	}
	printf( "\n" );
#endif

	/* CODIGO A OPTIMIZAR: Reducir matriz a triangular */
	/* Para cada elemento de la diagonal */
	int ti, tk, tj;
	double tmp1, tmp2, cout1, cout2;
	int numTiles = SIZE/BLOCK;

	for (ti=0; ti<numTiles; ti++) {
		for (tk=0; tk<numTiles; tk++) {		
			for (tj=0; tj<numTiles; tj++) {

				for (i=0; i<BLOCK; i++) {
					tmp1 = A[ti * BLOCK + i][ti * BLOCK + i];
					/* Actualizar otras files */
					for (k=i+1; k<BLOCK; k++) {
						tmp2 = A[tk * BLOCK + k][ti * BLOCK + i];
						for (j=i+1; j<BLOCK; j++) {
							cout1 = tmp1 * tmp2;
							cout2 = A[ti * BLOCK + i][tj * BLOCK + j] / cout1;
							A[tk * BLOCK + k][tj * BLOCK + j] -= cout2;
						}
						A[tk * BLOCK + k][ti * BLOCK + i] = 0;
					}

					/* Actualizar mi fila */
					for (j=i+1; j<BLOCK; j++) {
						A[ti * BLOCK + i][tj * BLOCK + j] = A[ti * BLOCK + i][tj * BLOCK + j] / tmp1;
					}

					A[ti * BLOCK + i][ti * BLOCK + i] = 1;
				}
			}
		}
	}

#ifdef DEBUG
	/* NO ALTERAR: Codigo para depuracion */
	for (i=0; i<SIZE; i++) {
		for (j=0; j<SIZE; j++) {
			printf("%8.4lf ", A[i][j] );
		}
		printf( "\n" );
	}
#endif

	/* Usar el resultado para que el optimizador no elimine
	 * el código que lo calcula
	 */
	double checksum = 0.0;
	for (i=0; i<SIZE; i++)
		for (j=0; j<SIZE; j++)
			checksum += A[i][j];

	printf("Checksum: %12.8lf\n", checksum );

	return 0;
}
