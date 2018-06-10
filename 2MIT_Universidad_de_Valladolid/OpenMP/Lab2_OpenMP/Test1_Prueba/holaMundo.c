#include<stdio.h>
#include<omp.h>

/* 1. Include de biblioteca OpenMP */

int main() {

	/* 2. Directiva de paralelismo */
	#pragma omp parallel
	/* 3. Escribir el identificador de thread y la cantidad de threads */
	printf("Hola mundo %d \n", omp_get_thread_num());

	return 0;
}
