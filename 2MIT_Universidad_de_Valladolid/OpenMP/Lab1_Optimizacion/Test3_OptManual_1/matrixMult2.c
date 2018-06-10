
#include<stdio.h>
#include<stdlib.h>

#define SIZE_MAT	1024
#define SIZE_ARRAY	1024
#define SEED	6834723

int main() {
	int i,j,k;

	double A[ SIZE_ARRAY ][ SIZE_ARRAY ];
	double B[ SIZE_ARRAY ][ SIZE_ARRAY ];
	double C[ SIZE_ARRAY ][ SIZE_ARRAY ];

	srand48( SEED );

	for (i=0; i<SIZE_MAT; i++)
		for (j=0; j<SIZE_MAT; j++) {
			C[i][j] = 0;
			A[i][j] = drand48();
			B[i][j] = drand48();
		}

	for (i=0; i<SIZE_MAT; i++)
		for (j=0; j<SIZE_MAT; j++) 
			for (k=0; k<SIZE_MAT; k=k+1)  
				C[i][j] = C[i][j] + A[i][k] * B[k][j];

	printf("Fin: %lf\n", C[0][0]);
	return 0;

}
