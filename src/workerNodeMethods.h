/*
File:    workerNodeMethods.h.
Purpose: Contains methods specific to all nodes other than head for file seperation between workers.
Author:  Thomas Goff MTSU <tag4c@mtmail.mtsu.edu>

*/

/*
==================================================

Required Header Files

==================================================
*/

#include <string>
#include <vector>
#include <dirent.h>
#include <cmath>
#include <iostream>
#include <mpi.h>

#ifndef DATA_H
#define DATA_H

struct dataStruct
{
	long long int id;  // Line Number
	double coordinates[3];     // column 1
};
#endif

#ifndef DATA_L
#define DATA_L

//struct for neighbor points
struct point
{
  double coordinates[3];
};

#endif

void decodeFilesToRead(int &fileEachNodeSize, std::vector<int> &localFileList, std::vector <std::string> &fileList, std::string path);
void recvFilesToRead(int &fileNodeEachSize, MPI_Status &status, std::vector<std::string> &localFileList);
void sendLocalPercentile(int &worldSize, std::vector <double> &localPercentile,int &numOfBins);
void recvGlobalPositionValue(std::vector <double> &globalPositionValueData);
void swapDataWorker(int &worldSize, std::vector<std::vector <dataStruct>> &dataArray, int &myrank,int **posIndex, int *filesPerNode);
void sendNeighPoints(std::vector<int>& numOfNeighPoints, int& linesToRead);
