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
	/* CODE TO OPTIMIZE: Reduce matrix to triangular */
	double m;
	/* Para cada elemento de la diagonal */
	/* For each element of the diagonal */
	for (i=SIZE-1; i>0; i--) {

		/* Actualizar otras files */
		/* Update other files */
		for (j=i-1; j>=0; j--) {
			m = A[j][i]/A[i][i];
			for (k=0; k<SIZE+1; k++) {
				A[j][k] = A[j][k] - (m * A[i][k]);

			}
		}

		/* Actualizar resto de columna */
		/* Update column rest */
		for (j=i+1; j<SIZE; j++) {
			A[j][i] = 0;
		}

		/* Actualizar mi fila */
		/* Update my row */
		for (k=i+1; k<SIZE; k++) {
			A[i][k] = A[i][k] / A[i][i];
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
	printf("Ultimo valor de la matriz: %8.4lf\n", A[SIZE-1][SIZE-1] );

	return 0;
}
