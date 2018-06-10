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
	#define SIZE	2880
#endif

#define SEED	6834723
#define TILESIZE 16

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
	int numTiles = SIZE/TILESIZE;
	int ti, tj, tk;
	/* Para cada elemento de la diagonal */
	for (ti=0; ti<numTiles; ti++) {
		for (tj=0; tj<numTiles; tj++) {
			for (tk=0; tk<numTiles; tk++) {
				for (i=0; i<TILESIZE; i++) {

					/* Actualizar otras files */
					for (k=i+1; k<TILESIZE; k++) {
						for (j=i+1; j<TILESIZE; j++) {
							A[tk*TILESIZE+k][tj*TILESIZE+j] = A[tk*TILESIZE+k][tj*TILESIZE+j] - A[ti*TILESIZE+i][tj*TILESIZE+j] / A[ti*TILESIZE+i][ti*TILESIZE+i] * A[tk*TILESIZE+k][ti*TILESIZE+i];

						}
					}

					/* Actualizar resto de columna */
					for (k=i+1; k<TILESIZE; k++) {
						A[tk*TILESIZE+k][ti*TILESIZE+i] = 0;
					}

					/* Actualizar mi fila */
					for (j=i+1; j<TILESIZE; j++) {
						A[ti*TILESIZE+i][tj*TILESIZE+j] = A[ti*TILESIZE+i][tj*TILESIZE+j] / A[ti*TILESIZE+i][ti*TILESIZE+i];
					}

					A[ti*TILESIZE+i][ti*TILESIZE+i] = 1;
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
