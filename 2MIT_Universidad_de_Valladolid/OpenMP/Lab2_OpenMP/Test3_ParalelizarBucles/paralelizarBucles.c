#include<stdio.h>
#include<stdlib.h>
/* 1. Include de biblioteca OpenMP */
#include<omp.h>

#define SIZE	200000000
int main() {
	double timeTot, timePar;

	timeTot = omp_get_wtime();

	int *v = (int *)malloc( SIZE * sizeof(int) );
	int i;
	int suma = 0;

	/* Inicializar */
	for ( i=0; i<SIZE; i++ ) v[i] = 0;

	/* 2. Directiva de bucle paralelo: Con vector v compartido e índice i privado */
	#pragma omp parallel private(i)
	#pragma omp parallel
	timePar = omp_get_wtime();
	for ( i=0; i<SIZE; i++ ) v[i] = i;
	timePar = omp_get_wtime() - timePar;

	/* Suma secuencial */
	for ( i=0; i<SIZE; i++ ) suma = ( suma + v[i] ) % 65535;
	timeTot = omp_get_wtime() - timeTot;
	printf( "Resultado final: %d\n", suma );
	printf("Total : %lf , Paralelo: %lf \n " , timeTot , timePar ) ;
	return 0;
}
