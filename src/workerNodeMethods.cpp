/*
File:    workerNodeMethods.cpp.
Purpose: Contains methods specific to all nodes other than head for file seperation between workers.
Author:  Thomas Goff MTSU <tag4c@mtmail.mtsu.edu>

*/

/*
==================================================

Required Header Files

==================================================
*/

#include "workerNodeMethods.h"


void decodeFilesToRead(int &fileEachNodeSize, std::vector<int> &localFileList, std::vector <std::string> &fileList)
{
	int i, j, k;
	std::string fileName = "datafiles/binary/output/datafile";

		for (i = 0; i < fileEachNodeSize; i++)
		{
			std::string temp = std::to_string(localFileList[i]);
			while (temp.length() < 5)
			{
				temp = "0" + temp;
			}
			temp = temp + ".bin";
			fileList[i] = fileName + temp;


		}
}

//recvFilesToRead(fileNodeEachSize, status, localFileList);

void recvFilesToRead(int &fileNodeEachSize, MPI_Status &status, std::vector<int> &localFileList)
{
		MPI_Recv(&fileNodeEachSize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		//std::vector<int> localFileList;
		localFileList.reserve(fileNodeEachSize);
		localFileList.resize(fileNodeEachSize);

		MPI_Recv(&localFileList.front(), fileNodeEachSize, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
}

void sendLocalPercentile(int &worldSize, std::vector <double> &localPercentile) 
{
		MPI_Send(&localPercentile.front(), worldSize - 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
}

void recvGlobalPositionValue(std::vector <double> &globalPositionValueData)
{
		int dataArraySize;
		MPI_Bcast(&dataArraySize, 1, MPI_INT, 0, MPI_COMM_WORLD);

		globalPositionValueData.reserve(dataArraySize);
		globalPositionValueData.resize(dataArraySize);
		MPI_Bcast(&globalPositionValueData.front(), dataArraySize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}
