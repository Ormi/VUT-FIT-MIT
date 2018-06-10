#include<stdio.h>
#include<omp.h>

/* 1. Include de biblioteca OpenMP */

int main() {

	int v[ 10 ];
	int i, x;

	/* Inicializar */
	for ( i=0; i<10; i++ ) v[i] = 0;
	/* 2. Directiva de paralelismo: Con vector v compartido */
	#pragma omp parallel shared( v )
	/* 3. Dentro de la región paralela: Cada thread escribe la posición
	 * 		de su identificador un 10 + su identificador */
	
	//v[ 0 ] = 10;
	//v[ 1 ] = 11;
	/* etc ... (Reescribir en paralelo) */
	#pragma omp parallel
	for ( x=0; x<10; x++) v[x] = 10 + x;

	/* Escribir */
	for ( i=0; i<10; i++ ) printf( " %d", v[i] );
	printf( "\n" );

	return 0;
}
