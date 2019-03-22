/*
File:    headNodeMethods.h.
Purpose: Contains methods specific to Head Node for file seperation between workers.
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
#include <mpi.h>
#include <iostream>

#ifndef DATA_H
#define DATA_H

struct dataStruct
{
	long long int id;  // Line Number
	double coordinates[3];     // column 1
};
#endif




void assignFilesToRead(std::string &dirpath, int worldSize, std::vector <std::vector<std::string>> &fileEachNode);
void read_directory(const std::string& name, std::vector<std::string>& v);
void decodeFilesToRead(int fileEachNodeSize, std::vector <std::vector<int>> &fileEachNode, std::vector <std::string> &fileList, std::string path);
void sendFilesToRead(int &worldSize, std::vector <std::vector<int>>  &fileEachNode, MPI_Request &request, int &fileEachNodeSize,int *filesPerNode);
void recvLocalPercentile(std::vector <double> &localPercentile, int &worldSize, MPI_Status &status, std::vector <std::vector <double>> &localPercentileList, int &numOfBins);
void sendGlobalPositionValue(int &arraySize, std::vector <double> &globalPositionValueData);
void swapDataHead(int &worldSize, std::vector<std::vector <dataStruct>> &dataArray, int &myrank, int **posIndex, int *filesPerNode);
