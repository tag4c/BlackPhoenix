/*
File:    headNodeMethods.cpp.
Purpose: Contains methods specific to Head Node for file seperation between workers.
Author:  Thomas Goff MTSU <tag4c@mtmail.mtsu.edu>

*/

/*
==================================================

Required Header Files

==================================================
*/

#include "headNodeMethods.h"

void decodeFilesToRead(int fileEachNodeSize, std::vector <std::vector<int>> &fileEachNode, std::vector <std::string> &fileList)
{
	int i, j, k;
	std::string fileName = "datafiles/binary/output/datafile";

		for (i = 0; i < fileEachNodeSize; i++)
		{
			std::string temp = std::to_string(fileEachNode[0][i]);
			while (temp.length() < 5)
			{
				temp = "0" + temp;
			}
			temp = temp + ".bin";
			fileList[i] = fileName + temp;
		}
}



void assignFilesToRead(std::string &dirpath, int worldSize, std::vector <std::vector<int>> &fileEachNode)
{
		int i, j, k;
		std::vector <int> dirList;

		read_directory(dirpath, dirList);

		int files_per_node = floor(dirList.size() / worldSize);
		int extra_files = dirList.size() % worldSize;

		int assigner = 0;

		int **file_range_per_node = new int*[worldSize];
		for (i = 0; i < worldSize; i++)
		{
			file_range_per_node[i] = new int[2];
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
		int tmp;
		std::vector<int> temp;
		int fileCount = 0;
		for (i = 0; i < worldSize; i++)
		{
			for (j = file_range_per_node[i][0]; j < file_range_per_node[i][1]+1; j++)
			{
				tmp = dirList[j];
				temp.push_back(tmp);
    			//std::cout << "Assigning file: " << tmp << " to node: " << i << std::endl;
				fileCount++;
			}

			fileEachNode.push_back(temp);
			temp.clear();

		}

    	// free up the array created by new

		for (i = 0; i < worldSize; i++)
		{
			delete file_range_per_node[i];
		}
		delete file_range_per_node;


}

void read_directory(const std::string& name, std::vector<int>& v)
{
	DIR* dirp = opendir(name.c_str());
	std::vector<std::string> tmp;
	struct dirent * dp;
	int tempInt = 0;
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
			std::string temp;
			temp = tmp[i].substr(8, 5);
			tempInt = stoi(temp);
			v.push_back(tempInt);
		}
	}


	closedir(dirp);
}

void sendFilesToRead(int &worldSize, std::vector <std::vector<int>>  &fileEachNode, MPI_Request &request, int &fileEachNodeSize)
{
	int i, j;
	/* This needs to be put in communication.cpp */

		/* ========================================*/
		//int fileEachNodeSize = 0;

		for (i = 1; i < worldSize; i++)
		{
			fileEachNodeSize = fileEachNode[i].size();

			MPI_Isend(&fileEachNodeSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
			MPI_Isend(&fileEachNode[i].front(), fileEachNodeSize, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
		}
		fileEachNodeSize = fileEachNode[0].size();


}

		
