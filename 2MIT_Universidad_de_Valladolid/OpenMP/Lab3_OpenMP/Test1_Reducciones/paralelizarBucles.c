#include<stdio.h>
#include<stdlib.h>
/* 1. Include de biblioteca OpenMP */

#define SIZE	200000000
int main() {

	int *v = (int *)malloc( SIZE * sizeof(int) );
	int i;
	int suma = 0;

	/* 1. Paralelizar la inicialización */
	for ( i=0; i<SIZE; i++ ) v[i] = i;

	/* 2. Paralelizar la suma, utilizando la cláusula de reducción */
	for ( i=0; i<SIZE; i++ ) suma = ( suma + v[i] ) % 65535;
	printf( "Resultado final: %d\n", suma );

	return 0;
}
