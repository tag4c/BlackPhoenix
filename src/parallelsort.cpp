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
#include "globalPosition.h"
#include "sperateArray.h"



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


using namespace std;


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
	MPI_Request request;
	MPI_Status status;


	/*  Create our MPI Datatype to pass our structure back and forth  */
	int blockSize[2] = { 1, 3 };
	MPI_Datatype MPI_dataStruct[2] = { MPI_INT, MPI_DOUBLE };
	MPI_Datatype MPI_dataArray;
	MPI_Aint offsets[2];

	offsets[0] = offsetof(dataStruct, id);
	offsets[1] = offsetof(dataStruct, coordinates);

	MPI_Type_create_struct(2, blockSize, offsets, MPI_dataStruct, &MPI_dataArray);

	MPI_Type_commit(&MPI_dataArray); // tell MPI we're done constructing our data type

	std::cout << "My rank: " << myrank << std::endl;

	/* Scheduler Node (HEAD NODE) */
	if (myrank == 0)
	{
		/* Have this node read all data in and send it out first? */
		std::vector <dataStruct> dataArray;
		std::string filepath = "../datafiles/binary/output/datafile00001.bin";
		
		//std::cout << filepath << std::endl;
		readFile(filepath, dataArray);
	//	std::cout << dataArray.size() << std::endl;
        sortPrep(dataArray, 0);
	/*	int arraySize = dataArray.size();
		std::cout << "Broadcasting size to All nodes!" << std::endl;
		MPI_Bcast(&arraySize, 1, MPI_INT, 0, MPI_COMM_WORLD);
		std::cout << "Broadcasting dataArray to all nodes!" << std::endl;
		MPI_Bcast(&dataArray.front(), arraySize, MPI_dataArray, 0, MPI_COMM_WORLD);
		std::cout << "Broadcasted to everyone!" << std::endl;
		for (i = 0; i < 1; i++)
		{

			std::cout << "Rank: " << myrank << " "<< dataArray[i].id << " " << std::setprecision(15) << dataArray[i].coordinates[0] << " " << dataArray[i].coordinates[1] << " " << dataArray[i].coordinates[2] << "\n";

		}
		*/
	//	std::cout << dataArray.size() << std::endl;
	//	writeFile(outfilepath, dataArray);
        vector <double> globalPositionValueData;
        vector <vector <double>> localPercentileList;
		vector <double> localPercentile(3);
        int numOfPercentiles = 3;
		int arraySize = dataArray.size();
        double numDataEachPart = 0.0;
		findPercentile(dataArray, numOfPercentiles, arraySize, columnToSort, localPercentile, numDataEachPart); 
		localPercentileList.push_back(localPercentile);
		// receive percentiles from everyone
		for (i = 1; i < worldSize; i++)
		{
			MPI_Recv(&localPercentile.front(), worldSize -1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);//, &request);
			localPercentileList.push_back(localPercentile);

		}

		std::cout << "HEAD: " << localPercentileList.size() << std::endl;

		for (i = 0; i < 3; i++)
		{
			std::cout << "HEAD: " << localPercentileList[i][0] << std::endl;
		}

		globalPositionValue(localPercentileList, worldSize, globalPositionValueData);
		
		std::cout << "HEAD: Size of gpv: " << globalPositionValueData.size() << std::endl;

		std::cout << "HEAD: " << globalPositionValueData[0] << " " << globalPositionValueData[1] << std::endl;

		/* Broadcast gpv data to all nodes */

		arraySize = globalPositionValueData.size();
		std::cout << "Broadcasting gpv data to All nodes!" << std::endl;
		MPI_Bcast(&arraySize, 1, MPI_INT, 0, MPI_COMM_WORLD);
		std::cout << "Broadcasting dataArray to all nodes!" << std::endl;
		MPI_Bcast(&globalPositionValueData.front(), arraySize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		std::cout << "Broadcasted to everyone!" << std::endl;

		arraySize = dataArray.size();
		//double numDataEachPart;
		vector <int> posIndex;

		sperateArray(dataArray, arraySize, globalPositionValueData, numDataEachPart, columnToSort, posIndex);

		std::cout << "HEAD: " << posIndex.size() << std::endl;

		std::cout << "HEAD: " << posIndex[0] << " " << posIndex[1] << " " << std::endl;







		// pass localPercentileList to globalPosition

		//std::cout << localPercentile[0] << " " << localPercentile[1] << std::endl;
	


		//std::cout << dataArray.size() << "\n";

}
	/* Slave nodes (All others) */
	else if (myrank == 1)
	{
		std::vector <dataStruct> dataArray;
		vector <double> globalPositionValueData;
		std::string filepath2 = "../datafiles/binary/output/datafile00003.bin";
		readFile(filepath2, dataArray);
		sortPrep(dataArray, 0);
		vector <double> localPercentile(3);
        int numOfPercentiles = 3;
		int arraySize = dataArray.size();
        double numDataEachPart = 0.0;
		findPercentile(dataArray, numOfPercentiles, arraySize, columnToSort, localPercentile, numDataEachPart); 
		/* Send local percentile to head node */

		MPI_Send(&localPercentile.front(), worldSize - 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD); //, &request);

		int dataArraySize;
		MPI_Bcast(&dataArraySize, 1, MPI_INT, 0, MPI_COMM_WORLD);
		std::cout << myrank<< "Received size!" << std::endl;
		std::cout << myrank<< "Recv: " << dataArraySize << std::endl;
		globalPositionValueData.reserve(dataArraySize);
		MPI_Bcast(&globalPositionValueData.front(), dataArraySize, MPI_DOUBLE, 0, MPI_COMM_WORLD);






	}
	else
	{
		std::vector <dataStruct> dataArray;
		std::string filepath2 = "../datafiles/binary/output/datafile00002.bin";
		vector <double> globalPositionValueData;
		readFile(filepath2, dataArray);
		sortPrep(dataArray, 0);
		vector <double> localPercentile(3);
        int numOfPercentiles = 3;
		int arraySize = dataArray.size();
        double numDataEachPart = 0.0;
		findPercentile(dataArray, numOfPercentiles, arraySize, columnToSort, localPercentile, numDataEachPart); 
		/* Send local percentile to head node */

		MPI_Send(&localPercentile.front(), worldSize - 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD); //, &request);

		int dataArraySize;
		MPI_Bcast(&dataArraySize, 1, MPI_INT, 0, MPI_COMM_WORLD);
		std::cout << myrank << "Received size!" << std::endl;
		std::cout << myrank << "Recv: " << dataArraySize << std::endl;
		globalPositionValueData.reserve(dataArraySize);
		MPI_Bcast(&globalPositionValueData.front(), dataArraySize, MPI_DOUBLE, 0, MPI_COMM_WORLD);





		/*std::cout << "In rank " << myrank << std::endl;
		std::vector <dataStruct> dataArray;
		int dataArraySize;
		MPI_Bcast(&dataArraySize, 1, MPI_INT, 0, MPI_COMM_WORLD);
		std::cout << "Received size!" << std::endl;
		std::cout << "Recv: " << dataArraySize << std::endl;
		dataArray.reserve(dataArraySize);
		MPI_Bcast(&dataArray.front(), dataArraySize, MPI_dataArray, 0, MPI_COMM_WORLD);
		for (i = 0; i < 1; i++)
		{

			std::cout << "Rank: " << myrank << " "<< dataArray[i].id << " " << std::setprecision(15) << dataArray[i].coordinates[0] << " " << dataArray[i].coordinates[1] << " " << dataArray[i].coordinates[2] << "\n";

		}
		*/
		//MPI_Recv(&dataArray, dataArraySize, MPI_dataArray, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);	
                //std::cout << dataArray[0].id << std::endl;
       //std::cout << dataArraySize << std::endl;
//       		for (i = 0; i < dataArraySize; i++)
	//	{
	//		std::cout << dataArray[i].id << " " << dataArray[i].coordinates[0] << " " << dataArray[i].coordinates[1] << " " << dataArray.coordinates[2] << std::endl;
	//	}
       		

	}







	MPI_Type_free(&MPI_dataArray); // clean up
	MPI_Finalize(); // clean up MPI usage


	return 0;


}




/* Main Routine function declarations  */ 


