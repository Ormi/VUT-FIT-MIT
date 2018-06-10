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

#define LEN		10
#define SEED	6834723
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

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
	double tmp1, tmp2, sum1, sum2;
	int x;
	/* Para cada elemento de la diagonal */
	for (i=0; i<SIZE; i++) {
		tmp1 = A[i][i];
		/* Actualizar otras files */
		for (k=i+1; k<SIZE; k++) {
			tmp2 = A[k][i];
			for (j=i+1; j<SIZE; j+=LEN) {
				for (x = j; x < MIN(SIZE, j + LEN); x++) {
					sum1 = A[i][x] / tmp1;
					sum2 = sum1 * tmp2;
					A[k][x] -= sum2;
				}
			}
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
