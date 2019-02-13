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
#include <time.h>

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
#include "headNodeMethods.h"
#include "workerNodeMethods.h"



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
	int i,j, k;
	int maxFiles;
	int maxNodes;
	int columnToSort = 0;
	int linesToRead = 0;
        clock_t start=clock();

	/* Variable initialization */
	//maxFiles = atoi(argv[1]);  // First command line argument for Number of files to read
	//maxNodes = atoi(argv[2]); // Second command line argument for number of compute nodes to use for the sort
	//columnToSort = atoi(argv[3]); // third command line argument - column  to sort on
	if (argc > 1)
	{
		linesToRead = atoi(argv[1]);
	}

	/* MPI Setup */
	int myrank, worldSize; // myrank - Node ID, worldSize - number of nodes available
	MPI_Init(&argc, &argv);
	char a;
	alarm(180);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

	/*  Create our MPI Datatype to pass our structure back and forth  */

	/* This might need to be in communication.cpp, I don't think it's needed in the main driver. */
	/* ========================================*/
	int blockSize[2] = { 1, 3 };
	MPI_Datatype MPI_dataStruct[2] = { MPI_INT, MPI_DOUBLE };
	MPI_Datatype MPI_dataArray;
	MPI_Aint offsets[2];

	offsets[0] = offsetof(dataStruct, id);
	offsets[1] = offsetof(dataStruct, coordinates);

	MPI_Type_create_struct(2, blockSize, offsets, MPI_dataStruct, &MPI_dataArray);

	MPI_Type_commit(&MPI_dataArray); // tell MPI we're done constructing our data type
	/* ========================================*/
	/* Scheduler Node (HEAD NODE) */
	if (myrank == 0)
	{
        	clock_t  t1,t2;
       		t1=start;
		/* Have this node read all data in and send it out first? */
		MPI_Request request;
		MPI_Status status;
		int fileEachNodeSize;
                ofstream timeData;
                timeData.open("timing.txt");
		//std::string dirpath = "/data/shared/shared/coms7900-data/BlackPhoenixBinary/";

		std::string dirpath = "/home/dtl2d/BlackPhoenix/datafiles/binary/output/";
		std::vector <std::vector<int>> fileEachNode;

		assignFilesToRead(dirpath, worldSize, fileEachNode); // determine number of files each node gets to read.

		sendFilesToRead(worldSize, fileEachNode, request, fileEachNodeSize);

		std::vector <std::string> fileList(fileEachNodeSize);


		decodeFilesToRead(fileEachNodeSize, fileEachNode, fileList);

		std::vector <dataStruct> dataArray;
		
		readFile(fileList[0], dataArray, linesToRead);
                t2=clock()-t1;
                t1=t2;
                timeData<<"time to read files from head:"<<(float(t2)/CLOCKS_PER_SEC)<<"s"<<endl;

		sortPrep(dataArray, columnToSort);
                t2=clock()-t1;
                t1=t2;
                timeData<<"time for local sort from head:"<<(float(t2)/CLOCKS_PER_SEC)<<"s"<<endl;


		vector <double> globalPositionValueData;
		vector <vector <double>> localPercentileList;
		vector <double> localPercentile(worldSize-1);
		int numOfPercentiles = worldSize;
		int arraySize = dataArray.size();
		double numDataEachPart = 0.0;
		findPercentile(dataArray, numOfPercentiles, arraySize, columnToSort, localPercentile, numDataEachPart); 
		localPercentileList.push_back(localPercentile);

		/* Receive local percentile data from worker nodes */

		recvLocalPercentile(localPercentile, worldSize, status, localPercentileList);

		/* Calculate global position data */

		globalPositionValue(localPercentileList, worldSize, globalPositionValueData);
		
		arraySize = globalPositionValueData.size();
		
		/* Broadcast gpv data to all nodes */

		sendGlobalPositionValue(arraySize, globalPositionValueData);

		vector <int> remotePosIndex;

		arraySize = dataArray.size();

		//double numDataEachPart;
		vector <int> posIndex;

		sperateArray(dataArray, arraySize, globalPositionValueData, numDataEachPart, columnToSort, posIndex);		

		MPI_Barrier(MPI_COMM_WORLD);
                t2=clock()-t1;
                t1=t2;
                timeData<<"time to find where data should go:"<<(float(t2)/CLOCKS_PER_SEC)<<"s"<<endl;



		swapDataHead(worldSize, dataArray, myrank, posIndex);
                t2=clock()-t1;
                t1=t2;
                timeData<<"time to send data:"<<(float(t2)/CLOCKS_PER_SEC)<<"s"<<endl;

		sortPrep(dataArray, columnToSort);
                t2=clock()-t1;
                t1=t2;
                timeData<<"final sort:"<<(float(t2)/CLOCKS_PER_SEC)<<"s"<<endl;

		int linecount = 0;
		std::string filepath = to_string(myrank) + "output.txt";
		std::ofstream file(filepath);

		//std::cout << "Size of dataArray: " << dataArray.size() << std::endl;
		// write new files

		if (file.is_open())
	{
		while ( linecount < dataArray.size() )
		{

            file << std::setprecision(15) << dataArray[linecount].coordinates[0] << "\n";
        //             std::cout << dataArray[linecount].id << " " << std::setprecision(15) << dataArray[linecount].coordinates[0] << " " << dataArray[linecount].coordinates[1] << " " << dataArray[linecount].coordinates[2] << "\n";
			linecount++;
		}

		file.close();


	}

		MPI_Barrier(MPI_COMM_WORLD);
                t2=clock()-start;
                timeData<<"final time from head node: "<<(float(t2)/CLOCKS_PER_SEC)<<"s"<<endl;

	}
	/* Slave nodes (All others) */
	else
	{
		MPI_Request request;
		MPI_Status status;
		//int columnToSort;
		int fileNodeEachSize = 0;

		std::vector<int> localFileList;

		recvFilesToRead(fileNodeEachSize, status, localFileList);

		std::vector <std::string> fileList(fileNodeEachSize);


		decodeFilesToRead(fileNodeEachSize, localFileList, fileList);

		std::vector <dataStruct> dataArray;
		vector <double> globalPositionValueData;

		readFile(fileList[0], dataArray, linesToRead);
		sortPrep(dataArray, columnToSort);

		vector <double> localPercentile(worldSize-1);
		int numOfPercentiles = worldSize;
		int arraySize = dataArray.size();
		double numDataEachPart = 0.0;
		findPercentile(dataArray, numOfPercentiles, arraySize, columnToSort, localPercentile, numDataEachPart); 

		sendLocalPercentile(worldSize, localPercentile); 

		recvGlobalPositionValue(globalPositionValueData);

		arraySize = dataArray.size();

		vector <int> posIndex;

		sperateArray(dataArray, arraySize, globalPositionValueData, numDataEachPart, columnToSort, posIndex);

		MPI_Barrier(MPI_COMM_WORLD);

		// this needs to goto workerNodeMethods
		// ========================================================
		swapDataWorker(worldSize, dataArray, myrank, posIndex);

		// ========================================================

		// sort

		sortPrep(dataArray, columnToSort);


		// write new files

		std::string filepath = to_string(myrank) + "output.txt";
		std::ofstream file(filepath);
		int linecount = 0;

	if (file.is_open())
	{
		while ( linecount < dataArray.size() )
		{

            file << std::setprecision(15) << dataArray[linecount].coordinates[0] << "\n";
			linecount++;
		}

		file.close();


	}

		MPI_Barrier(MPI_COMM_WORLD);
	}

	MPI_Type_free(&MPI_dataArray); // clean up
	MPI_Finalize(); // clean up MPI usage

	return 0;


}



/* Main Routine function declarations  */ 


