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




void assignFilesToRead(std::string &dirpath, int worldSize, std::vector <std::vector<int>> &fileEachNode);
void read_directory(const std::string& name, std::vector<int>& v);
void decodeFilesToRead(int fileEachNodeSize, std::vector <std::vector<int>> &fileEachNode, std::vector <std::string> &fileList);
void sendFilesToRead(int &worldSize, std::vector <std::vector<int>>  &fileEachNode, MPI_Request &request, int &fileEachNodeSize);