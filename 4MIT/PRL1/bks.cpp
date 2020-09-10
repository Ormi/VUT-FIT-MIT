/*
PRL Project 2
@author Michal Ormos
@email xormos00@stud.fit.vutbr.cz
@date March 2019
*/

#include <iostream>
#include <mpi.h>
#include <math.h>
#include <vector>
#include <string>
#include <fstream>
#include <time.h>
#include <algorithm>
#include <iterator>
using namespace std; 

#define DEBUG 0
#define TIME 0

/**
	Function for just displaying content of vector
	@param vector of int numbers
	@return display content to stdout
*/
void display_vector(const vector<int> &v)
{
    copy(v.begin(), v.end(),
        ostream_iterator<int>(cout, "\n"));
}

/**
	Main function, which
	1) Parse input file
	2) Logic of p=0
	3) Logic of all other nodes
*/
int main(int argc, char *argv[]) {
    int processesCount = 0;
    int sumOfNumbers = 0;
    int processId = 0;
    int bucketSize = 0;
    int leavesProcesses = 0;

    string fileName = "numbers";
	const int EOS=-1;

	// Initialize the MPI
    MPI_Init(&argc, &argv);
    // Get number of process
    MPI_Comm_size(MPI_COMM_WORLD,&processesCount); //get number of processes
    MPI_Comm_rank(MPI_COMM_WORLD,&processId); //get actual process id
   	double t1,t2;
    MPI_Status status;
    int MSG_SIZE;
  
    /**************** MASTER process, which controll all others ***************/
    if(processId == 0) {
       	////////////////////////////////////////////////////////////////////////
    	// READ from file
    	int num;
    	bool canRead = true;
    	bool firstNum = true;
    	vector<int> numbers;
        ifstream inputFile;
        inputFile.open(fileName.c_str()); 

        // Read until no characters is left
        while(!inputFile.eof()) {
        	num = inputFile.get();
       		if(num != -1) {
       			// get rid of space after last character
       			if(!firstNum) {
       				cout << " ";
       			}
       			firstNum = false;
       			cout << num;
       			numbers.push_back(num);
       			sumOfNumbers++;
       		}
        }
        cout << "\n";

        if(numbers.size() == 1) {
        	display_vector(numbers);
        	MPI_Finalize();
        	return 0;
        } else if(numbers.size() == 2) {
        	sort(numbers.begin(), numbers.end());
        	display_vector(numbers);
        	MPI_Finalize();
        	return 0;        
        }

        bucketSize = round(sumOfNumbers/((processesCount+1)/2));  
        leavesProcesses = (processesCount+1)/2;

        // Control prints
        if (DEBUG) {
	        cout << "\nNumber of numbers: " << sumOfNumbers << "\n"; 
	        cout << "Number of processes: " << processesCount << "\n";	
	        cout << "Bucket size: " << bucketSize << "\n";
	        cout << "Log2 number: " << leavesProcesses << "\n";
	        display_vector(numbers);
	    }
        //
       
       	/******************    Bucker Sort algorithm **************************/
       	////////////////////////////////////////////////////////////////////////
        // Start to transmiting numbers   	        	
		bool next = true;
		int  cCount = 0;	
		bool leftSon = true;
		bool rightSon = true;
	    int receivedDataLeft = 0;
	    int receivedDataRight = 0; 
    	int sendCounter = 0;
        int cpuCount = processesCount-leavesProcesses;
        int proc = cpuCount;

        // Send number by number to all availible sons
        for (int j = 0; j < sumOfNumbers; j++) {
        	MPI_Send(&numbers[j], 1, MPI_INT, proc, 0, MPI_COMM_WORLD);
        	sendCounter++;
        	if (proc == processesCount-1) {
        		proc = cpuCount;
        	} else {
        		proc++;
        	}
        }
        // last item which will be sent is signal of EOS stream
        for (int z = cpuCount; z <= processesCount; z++) {
	        MPI_Send(&EOS, 1, MPI_INT, z-1, 0, MPI_COMM_WORLD);	 
        }

       	////////////////////////////////////////////////////////////////////////
		// Start to receiving from others      
		vector<int> numbersBucketL;
		vector<int> numbersBucketR;
		vector<int> numbersBucket;   		  
		while(next) {
			if (leftSon) {
		    	MPI_Recv(&receivedDataLeft, 1, MPI_INT, (processId*2)+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		    }
		    if (rightSon) {
		    	MPI_Recv(&receivedDataRight, 1, MPI_INT, (processId*2)+2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		    }
		    if(DEBUG) {
		    	printf("Process %d received number %d %d\n", processId, receivedDataLeft, receivedDataRight);   
		    }	
		    // Receive data from left son	    
		    // and count how many EOS came
		    // we need 2, to be sure both sons send all data
		    if(receivedDataLeft != EOS) {		    
		    	numbersBucketL.push_back(receivedDataLeft);
		    } else if (leftSon) {
		    	if(cCount == 1) {
		    		next = false;
		    		cCount = 0;
		    		leftSon = true;
		    	} else {
		    		cCount++;
		    		leftSon = false;
		    	}
		    }	
		    // Receive data fom right son
		    if(receivedDataRight != EOS) {		    
		    	numbersBucketR.push_back(receivedDataRight);		    	
		    } else if (rightSon) {
		    	if(cCount == 1) {
		    		next = false;
		    		cCount = 0;
		    		rightSon = true;
		    	} else {
		    		cCount++;
		    		rightSon = false;
		    	}
		    }			
		}
		// Merge last two sons
		// and print final output
    	int size = (numbersBucketL.size()+numbersBucketR.size());
		numbersBucket.resize(size);	    	
    	merge(numbersBucketL.begin(), numbersBucketL.end(), numbersBucketR.begin(), numbersBucketR.end(),numbersBucket.begin());		

    	display_vector(numbersBucket);

    /****************************** SLAVE process *****************************/
    } else {
		vector<int> numbersBucketL;
		vector<int> numbersBucketR;
		vector<int> numbersBucket;        	        	
		bool next = true;
		int  cCount = 0;	
		bool leftSon = true;
		bool rightSon = true;
	    int receivedDataLeft = 0;
	    int receivedDataRight = 0;        
	    bool amILeavesProc;  

	    // Make a math, if I am leaves proces with no sons
	    // or I am a parent with sons?
        if(processId >= ((processesCount/2))) {
        	amILeavesProc = true;
        } else {
        	amILeavesProc = false;
        }

       	////////////////////////////////////////////////////////////////////////
        // Leaves receiving
        while(next && amILeavesProc) {
		    MPI_Recv(&receivedDataLeft, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		    if(receivedDataLeft != EOS) {
		    	numbersBucket.push_back(receivedDataLeft);
		    } else {
		    	next = false;
		    }
		}	

       	////////////////////////////////////////////////////////////////////////
		// Parents receiving
		while(next && !amILeavesProc) {
			if (leftSon) {
		    	MPI_Recv(&receivedDataLeft, 1, MPI_INT, (processId*2)+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		    }
		    if (rightSon) {
		    	MPI_Recv(&receivedDataRight, 1, MPI_INT, (processId*2)+2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		    }
		    if(DEBUG) {
		    	printf("Process %d received number %d %d\n", processId, receivedDataLeft, receivedDataRight);   
		    }		   
		  	// Receive data from left son	    
		    // and count how many EOS came
		    // we need 2, to be sure both sons send all data
		    if(receivedDataLeft != EOS) {		    
		    	numbersBucketL.push_back(receivedDataLeft);
		    } else if (leftSon) {
		    	if(cCount == 1) {
		    		next = false;
		    		cCount = 0;
		    		leftSon = true;
		    	} else {
		    		cCount++;
		    		leftSon = false;
		    	}
		    }	
		    // Receive dara from right son
		    if(receivedDataRight != EOS) {		    
		    	numbersBucketR.push_back(receivedDataRight);		    	
		    } else if (rightSon) {
		    	if(cCount == 1) {
		    		next = false;
		    		cCount = 0;
		    		rightSon = true;
		    	} else {
		    		cCount++;
		    		rightSon = false;
		    	}
		    }			    		
		}
		if (TIME) {
 			t1 = MPI_Wtime();
 		}
		// Leaves are sorting
		if (amILeavesProc) {
	    	sort(numbersBucket.begin(), numbersBucket.end());
	    } else {
	    // Parents are merging
	    	int size = (numbersBucketL.size()+numbersBucketR.size());
			numbersBucket.resize(size);	    	
	    	merge(numbersBucketL.begin(), numbersBucketL.end(), numbersBucketR.begin(), numbersBucketR.end(),numbersBucket.begin());
	    }
    
       	////////////////////////////////////////////////////////////////////////    
    	// Leaves/Parents are sending data to their parents
		for (int i = 0; i < numbersBucket.size(); ++i)
		{
			if(processId % 2 == 0) {
	       		MPI_Send(&numbersBucket[i], 1, MPI_INT, (processId/2)-1, 0, MPI_COMM_WORLD);
	       	}
	       	if(processId % 2 == 1) {
				MPI_Send(&numbersBucket[i], 1, MPI_INT, (processId/2), 0, MPI_COMM_WORLD);
	       	}
		}
		// Send character which represents end of numbers stream
		if(processId % 2 == 0) {
       		MPI_Send(&EOS, 1, MPI_INT, (processId/2)-1, 0, MPI_COMM_WORLD);
       	}
       	if(processId % 2 == 1) {
			MPI_Send(&EOS, 1, MPI_INT, (processId/2), 0, MPI_COMM_WORLD);
       	}
 
    }
    if (TIME) {
	 	t2 = MPI_Wtime();
		printf("MPI_Wtime(): %1.9f ms\n", (t2-t1)*1000);
	    fflush(stdout);
	}
    // releive all alocated processes
    MPI_Finalize();
    // finish programm
    return 0;    

}