#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<omp.h>

#define	NUMITER	3

int main(int argc, char *argv[]) {

	double tiempo = omp_get_wtime();

	int i;
	int myId;

	int flagPing = 0;

	#pragma omp parallel private(i, myId)
	{
		myId = omp_get_thread_num();

		#pragma omp sections nowait
		{
			#pragma omp section
			{
				for ( i=0; i<NUMITER; i++ ) {
					while( flagPing != 0 ) {
						#pragma omp flush
				   	} /* wait */
					fprintf(stdout,"[%d] PING %d\n", myId, i);
					sleep( 1 );
					flagPing = 1;
				}
			}

			#pragma omp section
			{
				for ( i=0; i<NUMITER; i++ ) {
					while( flagPing != 1 ) { 
						#pragma omp flush
					} /* wait */
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
