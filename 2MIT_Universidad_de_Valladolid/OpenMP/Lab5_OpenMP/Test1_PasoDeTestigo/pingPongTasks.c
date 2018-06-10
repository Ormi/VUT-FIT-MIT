#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<omp.h>

#define	NUMITER	3

int main(int argc, char *argv[]) {

	double tiempo = omp_get_wtime();

	#pragma omp parallel
	{
		int i;
		int myId = omp_get_thread_num();
		#pragma omp single
		for ( i=0; i<NUMITER; i++ ) {

			{
				#pragma omp task depend(out:i)
				{
					fprintf(stdout,"[%d] PING %d\n", myId, i);
					sleep( 1 );
				}

				#pragma omp task depend(in:i)
				{
					fprintf(stdout,"[%d] PONG %d\n", myId, i);
					sleep( 1 );
				}
			}
		}
		#pragma omp taskwait

		fprintf(stdout,"[%d] Finalizado\n", myId);
	}

	tiempo = omp_get_wtime() - tiempo;
	printf("Tiempo final: %lf\n", tiempo);

	return 0;
}
