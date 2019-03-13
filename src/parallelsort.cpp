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
	long long int id;  // Line Number
	double coordinates[3];     // column 1
};
#endif

#ifndef DATA_N
#define DATA_N
struct node {
	int id;
	int below;
	double cent[3];
	double min[3];
	double max[3];
	double length;
};
#endif

using namespace std;


/* Main Routine */

int main(int argc, char *argv[])
{
	/* Variable Declarations */
	int i, j, k;
	char b;
	int maxFiles;
	int maxNodes;
	int columnToSort = 0;
	int linesToRead = 0;
	std::string path;
	clock_t start = clock();

	/* Variable initialization */
	//maxFiles = atoi(argv[1]);  // First command line argument for Number of files to read
	//maxNodes = atoi(argv[2]); // Second command line argument for number of compute nodes to use for the sort
	//columnToSort = atoi(argv[3]); // third command line argument - column  to sort on
	if (argc > 1)
	{
		linesToRead = atoi(argv[1]);
		path = argv[2];
	}

	/* MPI Setup */
	int myrank, worldSize; // myrank - Node ID, worldSize - number of nodes available
	MPI_Init(&argc, &argv);
	char a;
	//alarm(180);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

	/*  Create our MPI Datatype to pass our structure back and forth  */

	/* This might need to be in communication.cpp, I don't think it's needed in the main driver. */
	/* ========================================*/
	int blockSize[2] = { 1, 3 };
	MPI_Datatype MPI_dataStruct[2] = { MPI_LONG_LONG_INT, MPI_DOUBLE };
	MPI_Datatype MPI_dataArray;
	MPI_Aint offsets[2];

	offsets[0] = offsetof(dataStruct, id);
	offsets[1] = offsetof(dataStruct, coordinates);

	MPI_Type_create_struct(2, blockSize, offsets, MPI_dataStruct, &MPI_dataArray);

	MPI_Type_commit(&MPI_dataArray); // tell MPI we're done constructing our data type
	/* ========================================*/
	/* Scheduler Node (HEAD NODE) */
	const int mbins = 10;
	const float scale = 1.05;
	if (myrank == 0)
	{
		clock_t  t1, t2;
		t1 = start;
		/* Have this node read all data in and send it out first? */
		MPI_Request request;
		MPI_Status status;
		int fileEachNodeSize;
                int *filesPerNode;
                int fileNum=0;
                filesPerNode=new int [worldSize];
		ofstream timeData;
		timeData.open("timing.txt");
		//std::string dirpath = "/data/shared/shared/coms7900-data/BlackPhoenixBinary/";

		//std::string dirpath = "/home/dtl2d/BlackPhoenix/datafiles/binary/output/";
		std::string dirpath = path;
		std::vector <std::vector<int>> fileEachNode;

		assignFilesToRead(dirpath, worldSize, fileEachNode); // determine number of files each node gets to read.

		sendFilesToRead(worldSize, fileEachNode, request, fileEachNodeSize, filesPerNode);
 
                for(int i=0;i<worldSize;i++){
                   fileNum=fileNum+filesPerNode[i];
                } 

		std::vector <std::string> fileList(fileEachNodeSize);


		decodeFilesToRead(fileEachNodeSize, fileEachNode, fileList, path);
                mpi_Bcast(filesPerNode,worldSize,MPI_INT,0,MPI_COMM_WORLD)
		std::vector<std::vector <dataStruct>> dataArrayList;
		dataArrayList.reserve(fileList.size());
		vector <vector <double>> localPercentileList(fileList.size());
		int numOfPercentiles = mbins * worldSize;
		double numDataEachPart = 0.0;


		for (i = 0; i < fileList.size(); i++)
		{

			int arraySize = dataArrayList[i].size();
			vector <double> localPercentile(mbins * worldSize - 1);
			readFile(fileList[i], dataArrayList[i], linesToRead); // read
			sortPrep(dataArrayList[i], columnToSort, 0, dataArrayList[i].size() - 1); // sort
			findPercentile(dataArrayList[i], numOfPercentiles, arraySize, columnToSort, localPercentileList[i], numDataEachPart);
		}

		vector <double> localGlobalPercentile(numOfPercentiles);

		globalPositionValue(localPercentileList, worldSize, localGlobalPercentile, numOfPercentiles);



		//  t2=clock()-t1;
		//t1=t2;
		// timeData<<"time to read files from head:"<<(float(t2)/CLOCKS_PER_SEC)<<"s"<<endl;

		//   cout<<dataArray[0].coordinates[0]<<endl;

		//cout<<dataArray[0].coordinates[0]<<endl;
		//t2=clock()-t1;
		//t1=t2;
		//timeData<<"time for local sort from head:"<<(float(t2)/CLOCKS_PER_SEC)<<"s"<<endl;


		vector <double> globalPositionValueData;

		vector < vector <double>> localGlobalPercentileList(worldSize);



		// cout<<dataArray[0].coordinates[0]<<endl;


		/* Receive local percentile data from worker nodes */

		recvLocalPercentile(localGlobalPercentile, worldSize, status, localGlobalPercentileList, numOfPercentiles);
		//	cout << "debug4\n";

		/* Calculate global position data */

		globalPositionValue(localGlobalPercentileList, worldSize, globalPositionValueData, numOfPercentiles);
		cout << "debug5\n";

		int arraySize = globalPositionValueData.size();

		/* Broadcast gpv data to all nodes */

		sendGlobalPositionValue(arraySize, globalPositionValueData);
		cout << "debug6\n";

		vector <int> remotePosIndex;

		vector <vector <int>> posIndexList(fileList.size());

		// sperate array loop

		for (i = 0; i < fileList.size(); i++)
		{
			int dataSize = dataArrayList[i].size();
			sperateArray(dataArrayList[i], dataSize, globalPositionValueData, numDataEachPart, columnToSort, posIndexList[i]);

		}

		//arraySize = dataArray.size();

		//double numDataEachPart;


		//cout << dataArray[0].coordinates[0] << endl;

		//cout << dataArray[0].coordinates[0] << endl;
		cout << "debug7\n";

		MPI_Barrier(MPI_COMM_WORLD);
		t2 = clock() - t1;
		t1 = t2;
		timeData << "time to find where data should go:" << (float(t2) / CLOCKS_PER_SEC) << "s" << endl;
		int *binSizes;
		int *binSizes2;
		binSizes = new int[numOfPercentiles];
		binSizes2 = new int[numOfPercentiles];
		for (int i = 0; i < numOfPercentiles; i++) {
			binSizes[i] = 0;

		}
                for(int j=0; j<filesPerNode[myrank];j++){
			for (int i = 0; i < numOfPercentiles; i++) {
				if (i > 0 && i < numOfPercentiles - 1)
					binSizes[i] = posIndexList[j][i] - posIndexList[j][i - 1];
				else if (i == 0)
					binSizes[i] = posIndexList[j][i];
				else
					binSizes[i] = linesToRead - posIndexList[j][i - 1];
			}
                }
    
		MPI_Barrier(MPI_COMM_WORLD);
		for (int i = 1; i < worldSize; i++) {
			MPI_Recv(binSizes2, numOfPercentiles, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			for (int j = 0; j < numOfPercentiles; j++) {
				binSizes[j] = binSizes[j] + binSizes2[j];
			}
		}
		int *boundries;
		boundries = new int[worldSize + 1];
		boundries[0] = 0;
		boundries[worldSize] = linesToRead;
		int temp = 0, index = 1;

		for (int i = 0; i < numOfPercentiles; i++) {
			if ((temp + binSizes[i]) < ((linesToRead * scale * fileNum)/worldSize)) {
				temp = temp + binSizes[i];
			} else {
				boundries[index] = i - 1;
				cout << "node " << index - 1 << " will get " << temp << " elements\n";
				index++;
				temp = binSizes[i];
			}
		}
		MPI_Bcast(boundries, worldSize + 1, MPI_INT, 0, MPI_COMM_WORLD);
		for (int i = 0; i < worldSize - 1; i++) {
			//cout<<"b"<<i+1<<"="<<posIndex[boundries[i+1]]<<endl;
			boundries[i + 1] = posIndexList[boundries[i + 1]];
		}

		// for(int i=0;i<worldSize;i++)
		//cout<<"node "<<i<<" gets "<<boundries[i]<<" to" <<boundries[i+1]<<" from "<<myrank<<endl;

		swapDataHead(worldSize, dataArray, myrank, boundries,filesPerNode);
		t2 = clock() - t1;
		t1 = t2;
		timeData << "time to send data:" << (float(t2) / CLOCKS_PER_SEC) << "s" << endl;

		sortPrep(dataArray, columnToSort, 0, dataArray.size() - 1);
		t2 = clock() - t1;
		t1 = t2;
		timeData << "final sort:" << (float(t2) / CLOCKS_PER_SEC) << "s" << endl;

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
		t2 = clock() - start;
		timeData << "final time from head node: " << (float(t2) / CLOCKS_PER_SEC) << "s" << endl;

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


		decodeFilesToRead(fileNodeEachSize, localFileList, fileList, path);
 
                mpi_Bcast(filesPerNode,worldSize,MPI_INT,0,MPI_COMM_WORLD)

		std::vector <dataStruct> dataArray;

		vector <double> globalPositionValueData;

		readFile(fileList[0], dataArray, linesToRead);

		sortPrep(dataArray, columnToSort, 0, dataArray.size() - 1);

		vector <double> localPercentile(mbins * worldSize - 1);
		int numOfPercentiles = mbins * worldSize;
		int arraySize = dataArray.size();
		double numDataEachPart = 0.0;
		findPercentile(dataArray, numOfPercentiles, arraySize, columnToSort, localPercentile, numDataEachPart);

		sendLocalPercentile(worldSize, localPercentile, numOfPercentiles);

		recvGlobalPositionValue(globalPositionValueData);

		arraySize = dataArray.size();

		vector <int> posIndex;

		sperateArray(dataArray, arraySize, globalPositionValueData, numDataEachPart, columnToSort, posIndex);

		MPI_Barrier(MPI_COMM_WORLD);

		// this needs to goto workerNodeMethods
		// ========================================================
		int *binSizes;
		binSizes = new int[numOfPercentiles];
                for(int j=0; j<filesPerNode[myrank];j++){
			for (int i = 0; i < numOfPercentiles; i++) {
				if (i > 0 && i < numOfPercentiles - 1)
					binSizes[i] = posIndexList[j][i] - posIndexList[j][i - 1];
				else if (i == 0)
					binSizes[i] = posIndexList[j][i];
				else
					binSizes[i] = linesToRead - posIndexList[j][i - 1];
			}
                }
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Send(binSizes, numOfPercentiles, MPI_INT, 0, 0, MPI_COMM_WORLD);
		int *boundries;
		boundries = new int[worldSize + 1];
		MPI_Bcast(boundries, worldSize + 1, MPI_INT, 0, MPI_COMM_WORLD);
		for (int i = 0; i < worldSize - 1; i++) {
			//cout<<"b"<<i+1<<"="<<posIndex[boundries[i+1]]<<endl;
			boundries[i + 1] = posIndex[boundries[i + 1]];
		}
		swapDataWorker(worldSize, dataArray, myrank, boundries, filesPerNode);

		// ========================================================

		// sort

		sortPrep(dataArray, columnToSort, 0, dataArray.size() - 1);


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


