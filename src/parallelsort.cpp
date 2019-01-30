/* Black Phoenix Parallel Sort main routine    */
// Some group information here...


/* Header files */

#include <fstream>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <iomanip>
#include <unistd.h>
#include "mpi.h"

// Common library includes for everyone...

// Specific includes for individually authored code

//
//
#include "readData.h"
#include "sortPrep.h"
#include "writeData.h"
#include "findPercentile.h"



// Functions needed for header includes
/*
read in data
sort
local location data (percentiles)
collect data (send local location data to head node)
head node calculates global location data, makes bin assignments
head node broadcasts global location data to all slave nodes.
broadcast bin data
slave nodes assign data to bins.
value data exchanged (sending struct's)
All nodes sort
write results to appropriate files



*/



/* Function prototypes    */

// Common functions everyone will use



/* Data structure setup */
/*
Overall Data List is an array of structures of this format:
structure format:

long integer   |   double   |   double  |   double

*/
#ifndef DATA_H
#define DATA_H

struct dataStruct
{
	int id;  // Line Number
	double coordinates[3];     // column 1
};
#endif





/* Main Routine */

int main(int argc, char *argv[])
{
	/* Variable Declarations */
	int i,j,k;
	int maxFiles;
	int maxNodes;
	int columnToSort = 0;

	


	/* Variable initialization */
	//maxFiles = atoi(argv[1]);  // First command line argument for Number of files to read
	//maxNodes = atoi(argv[2]); // Second command line argument for number of compute nodes to use for the sort
	//columnToSort = atoi(argv[3]); // third command line argument - column  to sort on

	/* MPI Setup */
	int myrank, worldSize; // myrank - Node ID, worldSize - number of nodes available
	MPI_Init(&argc, &argv);
	char a;
	alarm(180);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
	MPI_Request req;
	MPI_Status status;


	/*  Create our MPI Datatype to pass our structure back and forth  */
	//int blockSize[2] = { 1, 3 };
	//MPI_Datatype MPI_dataStruct[2] = { MPI_INT, MPI_DOUBLE };
	//MPI_Datatype MPI_dataArray;
	//MPI_Aint offsets[2];

	//offsets[0] = offsetof(dataStruct, id);
	//offsets[1] = offsetof(dataStruct, coordinates);

	//MPI_Type_create_struct(2, blockSize, offsets, MPI_dataStruct, &MPI_dataArray);

	//MPI_Type_commit(&MPI_dataArray); // tell MPI we're done constructing our data type

	std::cout << "My rank: " << myrank << std::endl;

	/* Scheduler Node (HEAD NODE) */
	if (myrank == 0)
	{
		/* Have this node read all data in and send it out first? */
		std::vector <dataStruct> dataArray;
		std::string filepath = "../datafiles/binary/output/datafile00001.bin";
		std::string outfilepath = "output.txt";
		//std::cout << filepath << std::endl;
		readFile(filepath, dataArray);
	//	std::cout << dataArray.size() << std::endl;
                sortPrep(dataArray, 0);
	//	std::cout << dataArray.size() << std::endl;
	//	writeFile(outfilepath, dataArray);
		vector <double> localPercentile(3);
                int numOfPercentiles = 3;
		int arraySize = dataArray.size();
                double numDataEachPart = 0.0;
		findPercentile(dataArray, numOfPercentiles, arraySize, columnToSort, localPercentile, numDataEachPart); 	
		


		//std::cout << dataArray.size() << "\n";
/*		for (i = 0; i < 10; i++)
		{

			std::cout << dataArray[i].id << " " << std::setprecision(15) << dataArray[i].coordinates[0] << " " << dataArray[i].coordinates[1] << " " << dataArray[i].coordinates[2] << "\n";

		}
	}
*/
}
	/* Slave nodes (All others) */









	//MPI_Type_free(&MPI_dataArray); // clean up
	MPI_Finalize(); // clean up MPI usage


	return 0;


}




/* Main Routine function declarations  */ 


