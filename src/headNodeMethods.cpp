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

using namespace std;

void decodeFilesToRead(int fileEachNodeSize, std::vector <std::vector<int>> &fileEachNode, std::vector <std::string> &fileList, std::string path)
{
	int i, j, k;
	//std::string fileName = "/data/shared/shared/coms7900-data/BlackPhoenixBinary/datafile";

	std::string fileName = path + "datafile";
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



void assignFilesToRead(std::string &dirpath, int worldSize, std::vector <std::vector<std::string>> &fileEachNode)
{
	int i, j, k;
	int startSize;
	int rank;
	int fileNum;
	char a;
	std::vector <std::string> dirList;
	std::string temp;
	//std::cout << "calling read dir\n";
	read_directory(dirpath, dirList);
	startSize = 0;
	rank = 1;
	fileNum = 0;

	//std::cout << "finished reading file list.\n";

	while (startSize < dirList.size())
	{
		temp = dirList[startSize]; // get a file off the list.
	//	std::cout << "Assigning file: " << temp << " to node: " << rank - 1 << std::endl;
		fileEachNode[rank - 1].push_back(temp); // push that file onto specific one.
		temp.clear();
		if (rank % worldSize != 0)
		{
			rank++; // cycle to next node.
		}
		else
		{
			rank = 1; // we're on the last node, so we need to go back to the first
		}
		startSize++; // decrease total number of files that need to be assigned.
	}

	/*for (i = 0; i < worldSize; i++)
	{
		std::cout << "Rank: " << i << " was assigned: " << fileEachNode[i].size() << " files.\n";
	}
	std::cout << "File assignments\n";
	for (i = 0; i < worldSize; i++)
	{
		for (j = 0; j < fileEachNode[i].size(); j++)
		{
			std::cout << "Rank: " << i << " was assigned: " << fileEachNode[i][j] << std::endl;
		}

	}

	std::cin >> a;

	*/
}

void read_directory(const std::string& name, std::vector<std::string>& v)
{
	DIR* dirp = opendir(name.c_str());
	int i;
	struct dirent * dp;
	std::string temp;
	std::vector<std::string> tmp;
	while ((dp = readdir(dirp)) != NULL) {
		if (dp->d_name != "." || dp->d_name != "..") // ignore . and ..
		{
			tmp.push_back(dp->d_name); // add file to list.
		}
	}
	for (i = 0; i < tmp.size(); i++)
	{
		temp = tmp[i];
		if (temp != "." && temp != "..")
		{
			v.push_back(temp);
		}
		temp.clear();
	}

	closedir(dirp);
}

void sendFilesToRead(int &worldSize, std::vector <std::vector<string>>  &fileEachNode, MPI_Request &request, int &fileEachNodeSize, int *filesPerNode)
{
	int i, j, k;
	/* This needs to be put in communication.cpp */

	/* ========================================*/
	//int fileEachNodeSize = 0;
	char *fileName;
	fileName = new char[17];

	for (i = 1; i < worldSize; i++)
	{
		fileEachNodeSize = fileEachNode[i].size();
		filesPerNode[i] = fileEachNode[i].size();
		//std::cout << "fileeachnode size: " << fileEachNodeSize;	
		MPI_Isend(&fileEachNodeSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
		for (j = 0; j < fileEachNode[i].size(); j++)
		{
			//fileName = fileEachNode[i][j].c_str();
			MPI_Isend(fileEachNode[i][j].c_str(), 17, MPI_CHAR, i, 0, MPI_COMM_WORLD, &request);

		}


	}
	fileEachNodeSize = fileEachNode[0].size();
	delete fileName;

}


void recvLocalPercentile(std::vector <double> &localPercentile, int &worldSize, MPI_Status &status, std::vector <std::vector <double>> &localPercentileList, int &numOfBins)
{
	int i;
	for (i = 1; i < worldSize; i++)
	{
		MPI_Recv(&localPercentile.front(), numOfBins - 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status); //, &request);
		localPercentileList.push_back(localPercentile);

	}
}

void sendGlobalPositionValue(int &arraySize, std::vector <double> &globalPositionValueData)
{
	MPI_Bcast(&arraySize, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&globalPositionValueData.front(), arraySize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

void swapDataHead(int &worldSize, std::vector<std::vector <dataStruct>> &dataArray, int &myrank, int **posIndex, int *filesPerNode)
{
	int blockSize[2] = { 1, 3 };
	MPI_Datatype MPI_dataStruct[2] = { MPI_LONG_LONG_INT, MPI_DOUBLE };
	MPI_Datatype MPI_dataArray;
	MPI_Aint offsets[2];

	offsets[0] = offsetof(dataStruct, id);
	offsets[1] = offsetof(dataStruct, coordinates);

	MPI_Type_create_struct(2, blockSize, offsets, MPI_dataStruct, &MPI_dataArray);

	MPI_Type_commit(&MPI_dataArray); // tell MPI we're done constructing our data type
	int i, j, k, max = 0;
	MPI_Request request0;
	MPI_Request request1;
	MPI_Request request2;
	MPI_Request request3;

	MPI_Status  status0;
	MPI_Status  status1;
	MPI_Status  status2;
	MPI_Status  status3;
	for (i = 0; i < worldSize; i++) {
		if (max < filesPerNode[i])
			max = filesPerNode[i];
	}
	int x = 1;
	int right = myrank;
	int left = myrank;
	int lNewSize, rNewSize;
	int rStart, lStart, rEnd, lEnd, rLength, lLength;
	vector <vector  <dataStruct>> newData(worldSize);
	vector <dataStruct> tempDataR;
	vector <dataStruct> tempDataL;
	for (k = 0; k <= max; k++) {
		x = 1;
		right = myrank;
		left = myrank;
		while (x <= (worldSize) / 2) {

			right++;
			if (right > worldSize - 1)
				right = 0;
			left--;

			if (left < 0)
				left = worldSize - 1;


			if (filesPerNode[myrank] - k > 0) {
				rStart = posIndex[k][right] + 1;

				if (right == worldSize - 1)
					rEnd = dataArray[k].size() - 1;

				else
					rEnd = posIndex[k][right + 1];

				rLength = rEnd - rStart + 1;


//		       cout<<myrank<<" sending "<<rLength<<" to "<<right << " from " << rStart << " to  " <<rEnd <<endl;
				MPI_Isend(&rLength, 1, MPI_INT, right, 0, MPI_COMM_WORLD, &request0);
				MPI_Isend(&dataArray[k][rStart], rLength, MPI_dataArray, right, 0, MPI_COMM_WORLD, &request1);
			}
			if (left != right && filesPerNode[myrank] - k > 0) {

				lStart = posIndex[k][left] + 1;

				if (left == worldSize - 1)
					lEnd = dataArray[k].size() - 1;

				else
					lEnd = posIndex[k][left + 1];

				lLength = lEnd - lStart + 1;

				//	       cout<<myrank<<" sending "<<lLength<<" to "<<left << " " << lStart << " " <<lEnd <<endl;
				MPI_Isend(&lLength, 1, MPI_INT, left, 0, MPI_COMM_WORLD, &request2);
				MPI_Isend(&dataArray[k][lStart], lLength, MPI_dataArray, left, 0, MPI_COMM_WORLD, &request3);



			}
			if (filesPerNode[right] - k > 0) {

				MPI_Recv(&rNewSize, 1, MPI_INT, right, 0, MPI_COMM_WORLD, &status0);

				tempDataR.reserve(rNewSize);
				tempDataR.resize(rNewSize);

				MPI_Recv(&tempDataR.front(), rNewSize, MPI_dataArray, right, 0, MPI_COMM_WORLD, &status1);
				for (i = 0; i < rNewSize; i++) {
					newData[right].push_back(tempDataR[i]);
				}
				tempDataR.reserve(0);
				tempDataR.resize(0);

			}
			if (left != right && filesPerNode[left] - k > 0) {

				MPI_Recv(&lNewSize, 1, MPI_INT, left, 0, MPI_COMM_WORLD, &status2);

				tempDataL.reserve(lNewSize);
				tempDataL.resize(lNewSize);

				MPI_Recv(&tempDataL.front(), lNewSize, MPI_dataArray, left, 0, MPI_COMM_WORLD, &status3);
				for (i = 0; i < lNewSize; i++) {
					newData[left].push_back(tempDataL[i]);
				}
				tempDataL.reserve(0);
				tempDataL.resize(0);

			}

			if (filesPerNode[myrank] - k > 0) {
				MPI_Wait(&request0, &status0);
				MPI_Wait(&request1, &status1);
			}
			if (left != right && filesPerNode[myrank] - k > 0) {

				MPI_Wait(&request2, &status2);
				MPI_Wait(&request3, &status3);

			}

			x++;
			MPI_Barrier(MPI_COMM_WORLD);
//			cout<<endl<<endl;
			MPI_Barrier(MPI_COMM_WORLD);

		}

	}

	// copy vector selection
	for (j = 0; j < filesPerNode[myrank]; j++) {
		for (k = 0; k < posIndex[j][1] + 1; k++)
		{
			newData[myrank].push_back(dataArray[j][k]);

		}
	}

	// clear old data

	//cout <<myrank<<" made it\n";
	for (k = 0; k < dataArray.size(); k++) {
		dataArray[k].clear();
		dataArray[k].shrink_to_fit();
	}
	dataArray.clear();
	dataArray.shrink_to_fit();
	dataArray.reserve(1);
	dataArray.resize(1);
	// make new vector from smaller ones

	for (j = 0; j < worldSize; j++)
	{
		for (k = 0; k < newData[j].size(); k++)
		{
			dataArray[0].push_back(newData[j][k]);
		}
	}
}


