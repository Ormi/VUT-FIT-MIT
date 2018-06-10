	// SIZE is from 5x5 but from [0,4]=5 elements
	int SIZE_MAX = (2*(SIZE+1))+(2*(SIZE+1)-2);

	//M[i,j] = ( M[i-1,j] + M[i,j-1] + M[i,j] ) / 3
	for (z = 0; z < SIZE_MAX ; z++) {

	printf("Iter: %d\n", z);

		//FIRST HALF
		if (z >= SIZE)
			MAX = SIZE;
		else
			MAX = z;
		if (z >= 2*SIZE+1)
			start = z - (2*SIZE+1);
		else 
			start = 0;

		for (a = start; a <= MAX; a++) {
			printf("U ");			
			for (b = 0; b <= a; b++) {
				printf("[%d,%d]", a-b, b );						
				if ((a-b) != 0 && b != 0) {		
					if ( z % 2 == 0 ) 	
						A[a-b][b] = (C[a-b-1][b] + C[a-b][b-1] + C[a-b][b]) / 3;
					else
						C[a-b][b] = (A[a-b-1][b] + A[a-b][b-1] + A[a-b][b]) / 3;
				}
			}
			printf("\n");
		}

		//MAIN anti-diagonal


		//SECOND HALF
		if (z >= SIZE) {
			if (z >= (4*SIZE)-1)
				start = z-(3*SIZE+1);
			else 
				start = 1;			
			for (a = start; a < z-SIZE+1; a++) {
				for (b = 0; b<(SIZE+1)-a; b++) {
					printf("[%d,%d]", SIZE-b, a+b );
					if ((SIZE-b) != 0 && (a+b) !=0) {
						if ( z % 2 != 0)
							A[SIZE-b][a+b] = (C[SIZE-b-1][a+b] + C[SIZE-b][a+b-1] + C[SIZE-b][a+b]) / 3;
						else 
							C[SIZE-b][a+b] = (A[SIZE-b-1][a+b] + A[SIZE-b][a+b-1] + A[SIZE-b][a+b]) / 3;
					}
				}
				printf("\n");	
			}			
		}	
	
	}