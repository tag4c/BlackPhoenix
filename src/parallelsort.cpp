/* Black Phoenix Parallel Sort main routine    */

/* Header files*/

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

#include "readData.h"
#include "sortPrep.h"
#include "writeData.h"
#include "findPercentile.h"
#include "globalPosition.h"
#include "sperateArray.h"
#include "headNodeMethods.h"
#include "workerNodeMethods.h"
#include "kdTree.h"

// header guards


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
	//char a;

	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

	/*  Create our MPI Datatype to pass our structure back and forth  */

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
		std::cout << "Starting head node\n";
		clock_t  t1, t2;
		t1 = start;
		/* Have this node read all data in and send it out first? */
		MPI_Request request;
		MPI_Status status;
		int fileEachNodeSize;
		int *filesPerNode;
		int fileNum = 0;
		filesPerNode = new int [worldSize];
		ofstream timeData;
		timeData.open("timing.txt");

		std::string dirpath = path;
		std::vector <std::vector<string>> fileEachNode(worldSize);

		assignFilesToRead(dirpath, worldSize, fileEachNode); // determine number of files each node gets to read.
		sendFilesToRead(worldSize, fileEachNode, request, fileEachNodeSize, filesPerNode);
		std::vector <std::string> fileList = fileEachNode[0];

		filesPerNode[0] = fileList.size();
		for (int i = 0; i < worldSize; i++) {
			fileNum = fileNum + filesPerNode[i];
		}
		MPI_Bcast(filesPerNode, worldSize, MPI_INT, 0, MPI_COMM_WORLD);
		std::vector<std::vector <dataStruct>> dataArrayList;
		//dataArrayList.reserve(fileList.size());
		vector <vector <double>> localPercentileList(fileEachNode[0].size());
		int numOfPercentiles = mbins * worldSize;
		//std::cout << numOfPercentiles << std::endl;
		double numDataEachPart = 0.0;
		//std::cout << "line 182\n";

		std::string tempFileName;
		//std::cout << "line147\n";
		for (i = 0; i < fileEachNode[0].size(); i++)
		{

			std::vector<dataStruct> tempDS;
			std::vector<double> tempd;
			//std::cout << i << std::endl;
			//tempd.clear();
			tempd.resize(numOfPercentiles - 1);
			//tempDS = dataArrayList[i];

			//vector <double> localPercentile(mbins * worldSize - 1);
			//std::cout << "line159\n" << std::endl;
			tempFileName = path + fileEachNode[0][i];
			//std::cout << tempFileName << std::endl;
			readFile(tempFileName, tempDS, linesToRead); // read
			int arraySize = tempDS.size();
			//std::cout << "line163\n" << std::endl;
			sortPrep(tempDS, columnToSort, 0, tempDS.size() - 1); // sort
			//std::cout << "size of tempDS: " << tempDS.size() << std::endl;
			//std::cout << "line166\n" << std::endl;

			//tempd = localPercentileList[i];
			findPercentile(tempDS, numOfPercentiles, arraySize, columnToSort, tempd, numDataEachPart);
			localPercentileList[i] = tempd;
		//	std::cout << "line 171\n";
			dataArrayList.push_back(tempDS);
			//	std::cout << "line210\n";
		}

		std::cout << "line208\n";

		vector <double> localGlobalPercentile;

		globalPositionValue(localPercentileList, worldSize, localGlobalPercentile, numOfPercentiles);
		std::cout << "line220\n";

		vector <double> globalPositionValueData;

		vector < vector <double>> localGlobalPercentileList;




		std::cout << localGlobalPercentile.size() << std::endl;

		localGlobalPercentileList.push_back(localGlobalPercentile);
		/* Receive local percentile data from worker nodes */

		recvLocalPercentile(localGlobalPercentile, worldSize, status, localGlobalPercentileList, numOfPercentiles);
		std::cout << "line246\n";
		int numOfBins = numOfPercentiles - 1;
		//	cout << "debug4\n";
		for (i = 0; i < worldSize; i++)
		{
			std::cout << localGlobalPercentileList[i].size() << std::endl;
		}

		/* Calculate global position data */

		globalPositionValue(localGlobalPercentileList, worldSize, globalPositionValueData, numOfBins);
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
		for (int j = 0; j < filesPerNode[myrank]; j++) {
			for (int i = 0; i < numOfPercentiles; i++) {
				if (i > 0 && i < numOfPercentiles - 1)
					binSizes[i] = binSizes[i] + posIndexList[j][i] - posIndexList[j][i - 1];
				else if (i == 0)
					binSizes[i] += posIndexList[j][i];
				else
					binSizes[i] += linesToRead - posIndexList[j][i - 1];
			}
		}
		//	cout << "debug7\n";
		//std::cout << "bin sizes[0]: " << binSizes[0] << std::endl;

		MPI_Barrier(MPI_COMM_WORLD);
		for (int i = 1; i < worldSize; i++) {
			MPI_Recv(binSizes2, numOfPercentiles, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			for (int j = 0; j < numOfPercentiles; j++) {
				//std::cout << i << "bin sizes2: " << binSizes2[j] << std::endl;
				binSizes[j] = binSizes[j] + binSizes2[j];
			}
			//std::cout << "bin sizes[" << i << "]: " << binSizes[i] << std::endl;
		}
		// last debug that shows.
		int *boundries;
		boundries = new int[worldSize + 1];
		int **boundries2;
		//std::cout << myrank << " " << filesPerNode[myrank] << std::endl;
		boundries2 = new int*[filesPerNode[myrank]];
		for (int i = 0; i < filesPerNode[myrank]; i++) {
			//std::cout << i << std::endl;
			boundries2[i] = new int[worldSize + 1];
			boundries2[i][0] = 0;
			boundries2[i][worldSize] = linesToRead;
		}
		//cout << "debug7\n";
		boundries[0] = 0;
		boundries[worldSize] = linesToRead;
		int temp = 0, index = 1;

		for (int i = 0; i < numOfPercentiles; i++) {
			if ((temp + binSizes[i]) < ((linesToRead * scale * fileNum) / worldSize)) {
				temp = temp + binSizes[i];
			} else {
				//	std::cout << "setting boundary...\n";
				boundries[index] = i - 1;
				//cout << "node " << index - 1 << " will get " << temp << " elements\n";
				index++;
				temp = binSizes[i];
			}
		}
		//for (i = 0; i < worldSize; i++)
		//{
		//	std::cout << myrank << " boundaries " << boundries[i] << std::endl;
		//	}
		//std::cout << "line351\n";
		MPI_Bcast(boundries, worldSize + 1, MPI_INT, 0, MPI_COMM_WORLD);
		//std::cout << "line353\n";
		for (int i = 0; i < filesPerNode[myrank]; i++) {
			for (j = 0; j < worldSize - 1; j++) {
				//cout<<"b"<<i+1<<"="<<posIndex[boundries[i+1]]<<endl;
				boundries2[i][j + 1] = posIndexList[i][boundries[j + 1]];
				//                std::cout<<"boundries "<<i <<" "<<j+1<<" "<<boundries2[i][j+1]<<std::endl;
			}
		}

		//std::cout << "line363\n";
		MPI_Barrier(MPI_COMM_WORLD);



		swapDataHead(worldSize, dataArrayList, myrank, boundries2, filesPerNode);
		t2 = clock() - t1;
		t1 = t2;
		timeData << "time to send data:" << (float(t2) / CLOCKS_PER_SEC) << "s" << endl;

		sortPrep(dataArrayList[0], columnToSort, 0, dataArrayList[0].size() - 1);
		std::cout << "done\n";
		t2 = clock() - t1;
		t1 = t2;
		timeData << "final sort:" << (float(t2) / CLOCKS_PER_SEC) << "s" << endl;

		// write new files

		//std::cout << dataArrayList[0].size()<< std::endl;
		/*
		for (j = 0; j < 1; j++)
		{
			int linecount = 0;
			std::string filepath = to_string(myrank) +  "output.txt";
			std::ofstream file(filepath);
			if (file.is_open())
			{
				while ( linecount < dataArrayList[j].size() )
				{

					file << std::setprecision(15) << dataArrayList[j][linecount].coordinates[0] << "\n";
					//             std::cout << dataArray[linecount].id << " " << std::setprecision(15) << dataArray[linecount].coordinates[0] << " " << dataArray[linecount].coordinates[1] << " " << dataArray[linecount].coordinates[2] << "\n";
					linecount++;
				}

				file.close();


			}

		}
		*/



		t2 = clock() - start;
		timeData << "final time from head node: " << (float(t2) / CLOCKS_PER_SEC) << "s" << endl;

		// Build KD Tree of sorted Data

		/* Create KD Tree */

		int layers = ceil(log2(1.0 * dataArrayList[0].size())) + 1;
		int treeMemSize = pow(2, layers);
		std::cout << treeMemSize << std::endl;
		node *tree;
		tree = new node[treeMemSize];

		kdTree(dataArrayList[0] , 1, 0, dataArrayList[0].size() - 1, tree);
	
		//std::cout << "line366\n";
		/*
				double *sp;
				sp = new double [3];

				sp[0] = 0.0;
				sp[1] = 0.0;
				sp[2] = 0.0;

				double radius = 0.1;

				vector<int> neighPoints(0);

				kdTree_search(tree, radius, sp, treeMemSize - 1, neighPoints);

				std::cout << "Node: " << myrank << " Number of Points with radius (" << radius << ") :" << neighPoints.size() << std::endl; */
		//verifySearch(dataArrayList[0], radius, sp);

		// Read in 501

		std::vector<dataStruct> searchDataArray;
		std::string filename = "/home/tag4c/datafile00501.bin";

		readFile(filename, searchDataArray, linesToRead);

//		std::cout << "line388\n";

		// sort it
		sortPrep(searchDataArray, columnToSort, 0, linesToRead-1);
		//std::cout << searchDataArray[0].coordinates[0] << std::endl;
		// send out 501
	//	std::cout << "line394\n";
		MPI_Bcast(&searchDataArray.front(), linesToRead, MPI_dataArray, 0, MPI_COMM_WORLD);

		//std::cout << myrank << ": done\n";
//std::cout << "line398\n";
		double *sp;
		sp = new double [3];
		double radius = 0.1;
		vector<vector<int>> neighPoints(linesToRead);
		vector<int> tempNeigh;
//		std::cout << "line406\n";
		for (i = 0; i < searchDataArray.size(); i++)
		{
			//std::cout << i << std::endl;
			sp[0] = searchDataArray[i].coordinates[0];
			sp[1] = searchDataArray[i].coordinates[1];
			sp[2] = searchDataArray[i].coordinates[2];
			kdTree_search(tree, radius, sp, treeMemSize - 1, tempNeigh);
			neighPoints[i] = tempNeigh;
		}

		std::cout << myrank << ": points within radius: " << neighPoints[0].size() << std::endl;


		// do something.. search...

		// for each line in 501, we need to find the number of points in the kd tree within a certain radius

		// print some stuff to file somewhere..
		//std::cout << "line423\n";
		MPI_Barrier(MPI_COMM_WORLD);
		std::cout << "head node done\n";


	}
	/* Slave nodes (All others) */
	else
	{
		std::cout << "Starting worker node.\n";
		MPI_Request request;
		MPI_Status status;
		//char a;
		//int columnToSort;
		int fileNodeEachSize = 0;
		int *filesPerNode;
		filesPerNode = new int [worldSize];

		std::vector<string> localFileList;

		recvFilesToRead(fileNodeEachSize, status, localFileList);

		std::vector <std::string> fileList(fileNodeEachSize);


		//decodeFilesToRead(fileNodeEachSize, localFileList, fileList, path);

		MPI_Bcast(filesPerNode, worldSize, MPI_INT, 0, MPI_COMM_WORLD);

		std::cout << "line 393\n";

		//std::vector <dataStruct> dataArray;
		std::vector < std::vector<dataStruct>> dataArrayList;
		//dataArrayList.reserve(fileList.size());
		vector <double> globalPositionValueData;

		vector <vector <double>> localPercentileList(localFileList.size());
		int numOfPercentiles = mbins * worldSize;
		double numDataEachPart = 0.0;

		std::string tempFileName;
		std::cout << "line 403\n";
		for (i = 0; i < localFileList.size(); i++)
		{
			std::vector<dataStruct> temp;
			std::vector<double> tempd;
			tempFileName = path + localFileList[i];
		//	std::cout << myrank << ":" << i<<std::endl;
			//	std::cout << "Worker: " <<  i << std::endl;
			//temp.clear();
			//temp.resize(dataArrayList[i].size());
			//tempd.clear();
			tempd.resize(numOfPercentiles - 1);
			//temp = dataArrayList[i];
			//int arraySize = temp.size();
			//	std::cout << "line411\n" << std::endl;
			//vector <double> localPercentile(mbins * worldSize - 1);
			//	std::cout << "line413\n" << std::endl;
			readFile(tempFileName, temp, linesToRead); // read
			//	std::cout << "line415\n" << std::endl;
			int arraySize = temp.size();
			sortPrep(temp, columnToSort, 0, temp.size() - 1); // sort
			//	std::cout << "line417\n" << std::endl;


			findPercentile(temp, numOfPercentiles, arraySize, columnToSort, tempd, numDataEachPart);
			localPercentileList[i] = tempd;
				std::cout << "line419\n" << std::endl;
			dataArrayList.push_back(temp);



		}

		vector <double> localGlobalPercentile;
		std::cout << "line499\n";
		globalPositionValue(localPercentileList, worldSize, localGlobalPercentile, numOfPercentiles);
		std::cout << "line500\n";
		sendLocalPercentile(worldSize, localGlobalPercentile, numOfPercentiles);
	std::cout << "line502\n";
		recvGlobalPositionValue(globalPositionValueData);
		int arraySize = dataArrayList.size();
	std::cout << "line505\n";
		vector <vector <int>> posIndexList(fileList.size());

		// sperate array loop

		for (i = 0; i < fileList.size(); i++)
		{
			int dataSize = dataArrayList[i].size();
			sperateArray(dataArrayList[i], dataSize, globalPositionValueData, numDataEachPart, columnToSort, posIndexList[i]);

		}
	std::cout << "line516\n";
		MPI_Barrier(MPI_COMM_WORLD);

		int *binSizes;
		binSizes = new int[numOfPercentiles];
		for (i = 0; i < numOfPercentiles; i++)
		{
			binSizes[i] = 0;
		}
		for (int j = 0; j < filesPerNode[myrank]; j++) {
			for (int i = 0; i < numOfPercentiles; i++) {
				if (i > 0 && i < numOfPercentiles - 1)
					binSizes[i] += posIndexList[j][i] - posIndexList[j][i - 1];
				else if (i == 0)
					binSizes[i] += posIndexList[j][i];
				else
					binSizes[i] += linesToRead - posIndexList[j][i - 1];

				//	std::cout << "binSize[" << i << "]" << binSizes[i] << std::endl;
			}
		}
		std::cout << "line528\n";
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Send(binSizes, numOfPercentiles, MPI_INT, 0, 0, MPI_COMM_WORLD);
		int *boundries;
		boundries = new int[worldSize + 1];
		MPI_Bcast(boundries, worldSize + 1, MPI_INT, 0, MPI_COMM_WORLD);

		int **boundries2;
		//std::cout << myrank << " " << filesPerNode[myrank] << std::endl;
		boundries2 = new int*[filesPerNode[myrank]];
		//std::cout << myrank << " " << posIndexList[myrank].size() << std::endl;
		for (int i = 0; i < filesPerNode[myrank]; i++) {

			boundries2[i] = new int[worldSize + 1];
			boundries2[i][0] = 0;
			boundries2[i][worldSize] = linesToRead;
		}
		for (int i = 0; i < filesPerNode[myrank]; i++) {
			//	std::cout << i << std::endl;
			//	std::cout << myrank << " : " << posIndexList[boundries[i]].size() << std::endl;
			for (j = 0; j < worldSize - 1; j++) {	//cout<<"b"<<i+1<<"="<<posIndex[boundries[i+1]]<<endl;

				boundries2[i][j + 1] = posIndexList[i][boundries[j + 1]];
				//std::cout<<"boundries "<<i <<" "<<j+1<<" "<<boundries2[i][j+1]<<std::endl;
			}

		}
		//std::cout << "line546\n";
		MPI_Barrier(MPI_COMM_WORLD);


		swapDataWorker(worldSize, dataArrayList, myrank, boundries2, filesPerNode);

		// sort

		sortPrep(dataArrayList[0], columnToSort, 0, dataArrayList[0].size() - 1);
		/*
				for (j = 0; j < 1; j++)
				{
					std::string filepath = to_string(myrank) + "output.txt";
					std::ofstream file(filepath);
					int linecount = 0;

					if (file.is_open())
					{
						while ( linecount < dataArrayList[j].size() )
						{
							file << std::setprecision(15) << dataArrayList[j][linecount].coordinates[0] << " " << dataArrayList[j][linecount].coordinates[1] << " " <<  dataArrayList[j][linecount].coordinates[2] <<  "\n";
							linecount++;
						}

						file.close();

					}

				}
		*/



		int layers = ceil(log2(1.0 * dataArrayList[0].size())) + 1;
		int treeMemSize = pow(2, layers);

		node *tree;
		tree = new node[treeMemSize];

		kdTree(dataArrayList[0] , 1, 0, dataArrayList[0].size() - 1, tree);
		
	//	std::cout << myrank << ": line581\n";

		double *sp;
		sp = new double [3];

		sp[0] = 0.0;
		sp[1] = 0.0;
		sp[2] = 0.0;

		double radius = 0.1;

		//vector<int> neighPoints(0);

		//kdTree_search(tree, radius, sp, treeMemSize - 1, neighPoints);

		//std::cout << "Node: " << myrank << " Number of Points with radius (" << radius << ") :" << neighPoints.size() << std::endl;

		// Receive data from head node in file 501.
		std::vector<dataStruct> searchDataArray;
		searchDataArray.reserve(linesToRead);
		searchDataArray.resize(linesToRead);


		MPI_Bcast(&searchDataArray.front(), linesToRead, MPI_dataArray, 0, MPI_COMM_WORLD);
		//std::cout << myrank << ": done\n";
		//searchDataArray.resize(linesToRead);
		//std::cout << searchDataArray[0].coordinates[0] << std::endl;

		// call kdtree_search on every point.
		/*double *sp;
		sp = new double [3];
		double radius = 0.1;*/
		vector<vector<int>> neighPoints(linesToRead);
		vector<int> tempNeigh;

		for (i = 0; i < searchDataArray.size(); i++)
		{
			sp[0] = searchDataArray[i].coordinates[0];
			sp[1] = searchDataArray[i].coordinates[1];
			sp[2] = searchDataArray[i].coordinates[2];
			kdTree_search(tree, radius, sp, treeMemSize - 1, tempNeigh);
			neighPoints[i] = tempNeigh;
		}
		std::cout << myrank << ": points within radius: " << neighPoints[0].size() << std::endl;

		// do someother stuff.

		//verifySearch(dataArrayList[0], radius, sp);

		// write new files

		//std::cout << "line633\n";

		MPI_Barrier(MPI_COMM_WORLD);
		std::cout << "node " << myrank << " done\n";
	}

	MPI_Type_free(&MPI_dataArray); // clean up
	MPI_Finalize(); // clean up MPI usage

	return 0;

}
