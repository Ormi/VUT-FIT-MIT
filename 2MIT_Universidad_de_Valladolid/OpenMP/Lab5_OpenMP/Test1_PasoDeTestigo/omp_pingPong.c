#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<omp.h>

#define	NUMITER	3

int main(int argc, char *argv[]) {

	double tiempo = omp_get_wtime();

	int flagPing = 0;

	#pragma omp parallel 
	{
		int i;
		int myId = omp_get_thread_num();
		for ( i=0; i<NUMITER; i++ ) {

			#pragma omp sections nowait
			{
				#pragma omp section
				{
					while( flagPing != 0 ) { } /* wait */
					fprintf(stdout,"[%d] PING %d\n", myId, i);
					sleep( 1 );
					flagPing = 1;
				}

				#pragma omp section
				{
					while( flagPing != 1 ) { } /* wait */
					fprintf(stdout,"[%d] PONG %d\n", myId, i);
					sleep( 1 );
					flagPing = 0;
				}
			}
		}

		fprintf(stdout,"[%d] Finalizado\n", myId);
	}

	tiempo = omp_get_wtime() - tiempo;
	printf("Tiempo final: %lf\n", tiempo);

	return 0;
}
