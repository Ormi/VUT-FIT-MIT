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
	for (i=0; i<SIZE; i++) {

		/* Actualizar otras files */
		for (k=i+1; k<SIZE; k++) {
			for (j=i+1; j<SIZE; j++) {
				A[k][j] = A[k][j] - A[i][j] / A[i][i] * A[k][i];

			}
		}

		/* Actualizar resto de columna */
		for (k=i+1; k<SIZE; k++) {
			A[k][i] = 0;
		}

		/* Actualizar mi fila */
		for (j=i+1; j<SIZE; j++) {
			A[i][j] = A[i][j] / A[i][i];
		}

		A[i][i] = 1;
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
