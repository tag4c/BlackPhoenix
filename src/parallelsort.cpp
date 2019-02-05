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
#include <dirent.h>
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




void read_directory(const std::string& name, std::vector<std::string>& v);

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

		std::string dirpath = "../datafiles/binary/output/";
		std::vector <std::string> dirList;

		std::vector <std::vector<std::string>> fileEachNode;

		read_directory(dirpath, dirList);

		// divvy up the files between nodes.

		// need an array or vector to hold each nodes file list..
		

		//std::cout << worldSize << std::endl;

		int files_per_node = floor(dirList.size() / worldSize);
		int extra_files = dirList.size() % worldSize;

		int assigner = 0;

		int **file_range_per_node = new int*[2];
		for (i = 0; i < worldSize; i++)
		{
			file_range_per_node[i] = new int;
		}
		for (i = 0; i < worldSize; i++)
    	{ 	
    	file_range_per_node[i][0] = assigner;
    	file_range_per_node[i][1] = assigner + files_per_node - 1;

    	if ((i == worldSize - 1) && file_range_per_node[i][1] <= dirList.size())
    	{
    		file_range_per_node[i][1] = dirList.size()-1;
    	}
    	assigner = assigner + files_per_node;
    	}

    	//std::cout << "Total number of files: " << dirList.size() << std::endl;
    	std::string tmp;
    	std::vector<string> temp;
    	int fileCount = 0;
    	for (i = 0; i < worldSize; i++)
    	{
    		for (j = file_range_per_node[i][0]; j < file_range_per_node[i][1]+1; j++)
    		{
    			tmp = dirList[j];
    			temp.push_back(tmp);
    			std::cout << "Assigning file: " << tmp << " to node: " << i << std::endl;
    			fileCount++;
    		}
    		fileEachNode.push_back(temp);
    		
    	}
    	//std::cout << fileCount << std::endl;

    	while (sentCount < fileCount)
    	{
    	
    		sendCount++;
    	}

		std::cin >> a;




		std::vector <dataStruct> dataArray;






		std::string filepath = "../datafiles/binary/output/datafile00001.bin";
		
	
		readFile(filepath, dataArray);

        sortPrep(dataArray, 0);
	

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

			vector <int> remotePosIndex;

		arraySize = dataArray.size();

		//double numDataEachPart;
		vector <int> posIndex;



		sperateArray(dataArray, arraySize, globalPositionValueData, numDataEachPart, columnToSort, posIndex);

		std::cout << "HEAD: " << posIndex.size() << std::endl;

		std::cout << "HEAD: " << posIndex[0] << " " << posIndex[1] << " " << std::endl;

		// Comm protocol

		/* 
		Even Case: 

		Send local position index array
		remote node does the math.. to figure out how much it's receiving.
		exchange data.
		Reverse roles.
		exchange data.

		while(x < nodes-1)
		if even -> myrank+x (sends data to this guy).. then receives from that guy

		if odd -> myrank - x (sends to this guy) .. recv from that guy


		next step... odd goes to myrank + x.
				     even goes to myrank - x

				     x++

				     continue... until x < node-1



		*/

		/* Send size of posIndex */

	
			/*if (worldSize % 2 == 0) // even
			{
					for (i = 0; i < (worldSize)/2; i++)
					{
						// send posIndex 
						MPI_Send(&posIndex[myrank], 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

					}


			}
			else
			{
				// odd.. 
			}*/
		




		// pass localPercentileList to globalPosition

}
	/* Slave nodes (All others) */
	else if (myrank == 1)
	{
		//int columnToSort;
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
		globalPositionValueData.resize(dataArraySize);
		MPI_Bcast(&globalPositionValueData.front(), dataArraySize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	

		arraySize = dataArray.size();

		std::cout << myrank << " GPV : " << globalPositionValueData[0] << " " << globalPositionValueData[1] << std::endl;

		std::cout << myrank << " GPV size: " << globalPositionValueData.size() << std::endl;


		//double numDataEachPart;
		vector <int> posIndex;



		sperateArray(dataArray, arraySize, globalPositionValueData, numDataEachPart, columnToSort, posIndex);

		std::cout << myrank << " " << posIndex.size() << std::endl;

		std::cout << myrank << " " << posIndex[0] << " " << posIndex[1] << " " << std::endl;

			/* Send size of posIndex */

		vector <int> remotePosIndex;








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
		std::cout << myrank<< "Received size!" << std::endl;
		std::cout << myrank<< "Recv: " << dataArraySize << std::endl;
		globalPositionValueData.reserve(dataArraySize);
		globalPositionValueData.resize(dataArraySize);
		MPI_Bcast(&globalPositionValueData.front(), dataArraySize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	

		arraySize = dataArray.size();

		std::cout << myrank << " GPV : " << globalPositionValueData[0] << " " << globalPositionValueData[1] << std::endl;

		std::cout << myrank << " GPV size: " << globalPositionValueData.size() << std::endl;


		//double numDataEachPart;
		vector <int> posIndex;



		sperateArray(dataArray, arraySize, globalPositionValueData, numDataEachPart, columnToSort, posIndex);

		std::cout << myrank << " " << posIndex.size() << std::endl;

		std::cout << myrank << " " << posIndex[0] << " " << posIndex[1] << " " << std::endl;

	//	posIndexSize = posIndex.size();

	//	vector <int> remotePosIndex;


			/* Send size of posIndex */

	/*	arraySize = dataArray.size();

		//double numDataEachPart;
		vector <int> posIndex;



		sperateArray(dataArray, arraySize, globalPositionValueData, numDataEachPart, columnToSort, posIndex);

		std::cout << myrank << " " << posIndex.size() << std::endl;

		std::cout << myrank << " " << posIndex[0] << " " << posIndex[1] << " " << std::endl;


*/


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

void read_directory(const std::string& name, std::vector<std::string>& v)
{
    DIR* dirp = opendir(name.c_str());
    std::vector<std::string> tmp;
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
    	if (dp->d_name != "." || dp->d_name != "..")
    	{
        	tmp.push_back(dp->d_name);
        }
    }
    for (int i = 0; i < tmp.size(); i++)
    {
    	if (tmp[i] == "." || tmp[i] == "..")
    	{
    		continue;
    	}
    	else
    	{
    		v.push_back(tmp[i]);
    	}
    }


    closedir(dirp);
}




/* Main Routine function declarations  */ 


