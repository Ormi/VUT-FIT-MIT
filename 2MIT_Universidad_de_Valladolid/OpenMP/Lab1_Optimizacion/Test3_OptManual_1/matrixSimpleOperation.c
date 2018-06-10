/*
 * Asignatura: Computación Paralela y Modelos Emergentes
 * Máster en Ingeniería Informática, Universidad de Valladolid
 *
 * Operación simple de matrices
 * Ejemplo de código secuencial a optimizar
 *
 * (c) Arturo Gonzalez-Escribano, 2017
 */
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define SIZE_MAT	8192
#define SEED	83487

double fooFunc( double a, double b ) {
	return sin(a) * cos(b);
}

int main() {
	int i,j,k;

	double *A = (double *)malloc( SIZE_MAT * SIZE_MAT * sizeof( double ) );
	double *B = (double *)malloc( SIZE_MAT * SIZE_MAT * sizeof( double ) );
	double *C = (double *)malloc( SIZE_MAT * SIZE_MAT * sizeof( double ) );
	if ( A == NULL || B == NULL || C == NULL ) {
		fprintf( stderr, "Error: Reserva de memoria\n" );
		exit( EXIT_FAILURE );
	}

	srand48( SEED );

	for (i=0; i<SIZE_MAT; i++)
		for (j=0; j<SIZE_MAT; j++) {
			C[i*SIZE_MAT+j] = 0;
			A[i*SIZE_MAT+j] = drand48();
			B[i*SIZE_MAT+j] = drand48();
		}

	for (j=0; j<SIZE_MAT; j++) 
		for (i=0; i<SIZE_MAT; i++)
			C[i*SIZE_MAT+j] = fooFunc( A[i*SIZE_MAT+j] , B[i*SIZE_MAT+j] );

	printf("Fin: %lf, %lf\n", *C, C[ SIZE_MAT*SIZE_MAT-1 ] );
	return 0;

}
