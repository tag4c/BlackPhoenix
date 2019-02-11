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
#include <mpi.h>

void decodeFilesToRead(int &fileEachNodeSize, std::vector<int> &localFileList, std::vector <std::string> &fileList);
void recvFilesToRead(int &fileNodeEachSize, MPI_Status &status, std::vector<int> &localFileList);
void sendLocalPercentile(int &worldSize, std::vector <double> &localPercentile);
void recvGlobalPositionValue(std::vector <double> &globalPositionValueData);