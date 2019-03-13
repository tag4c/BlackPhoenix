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

void sendFilesToRead(int &worldSize, std::vector <std::vector<int>>  &fileEachNode, MPI_Request &request, int &fileEachNodeSize, int *filesPerNode)
{
	int i, j;
	/* This needs to be put in communication.cpp */

		/* ========================================*/
		//int fileEachNodeSize = 0;

		for (i = 1; i < worldSize; i++)
		{
			fileEachNodeSize = fileEachNode[i].size();
                        filesPerNode[i]=fileEachNode[i].size();

			MPI_Isend(&fileEachNodeSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
			MPI_Isend(&fileEachNode[i].front(), fileEachNodeSize, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
		}
		fileEachNodeSize = fileEachNode[0].size();


}


void recvLocalPercentile(std::vector <double> &localPercentile, int &worldSize, MPI_Status &status, std::vector <std::vector <double>> &localPercentileList, int &numOfBins)
{
	int i;
	for (i = 1; i < worldSize; i++)
		{
			MPI_Recv(&localPercentile.front(), numOfBins-1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);//, &request);
			localPercentileList.push_back(localPercentile);

		}
}

void sendGlobalPositionValue(int &arraySize, std::vector <double> &globalPositionValueData)
{
		MPI_Bcast(&arraySize, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&globalPositionValueData.front(), arraySize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

void swapDataHead(int &worldSize, std::vector<std::vector <dataStruct>> &dataArray, int &myrank,int *posIndex, int *filesPerNode)
{
	int blockSize[2] = { 1, 3 };
	MPI_Datatype MPI_dataStruct[2] = { MPI_LONG_LONG_INT, MPI_DOUBLE };
	MPI_Datatype MPI_dataArray;
	MPI_Aint offsets[2];

	offsets[0] = offsetof(dataStruct, id);
	offsets[1] = offsetof(dataStruct, coordinates);

	MPI_Type_create_struct(2, blockSize, offsets, MPI_dataStruct, &MPI_dataArray);

	MPI_Type_commit(&MPI_dataArray); // tell MPI we're done constructing our data type
		int i, j, k,max=0;
		MPI_Request request0;
		MPI_Request request1;
		MPI_Request request2;
		MPI_Request request3;

		MPI_Status  status0;
		MPI_Status  status1;
		MPI_Status  status2;
		MPI_Status  status3;
                for(i=0;i<worldSize;i++){
                  if(max<filesPerNode[i])
                     max=filesPerNode[i];
                }
		int x=1; 
		int right=myrank;
		int left=myrank;
		int lNewSize,rNewSize;
		int rStart, lStart, rEnd, lEnd, rLength,lLength;
		vector <vector  <dataStruct>> newData(worldSize);
                vector <dataStruct> tempDataR;
                vector <dataStruct> tempDataL;
               for(k=0;k<=max;k++){       
                x=0;
		while(x<=(worldSize)/2){

			right++;
			if(right>worldSize-1)
				right=0;
			left--;

			if(left<0)
				left=worldSize-1;


                        if(filesPerNode[myrank]-x>0){
			rStart=posIndex[right]+1;

			if(right==worldSize-1)
				rEnd=dataArray.size()-1;

			else 
				rEnd=posIndex[right+1];

			rLength=rEnd-rStart+1; 
                         

		       //cout<<myrank<<" sending "<<rLength<<" to "<<right << " from " << rStart << " to  " <<rEnd <<endl;
			MPI_Isend(&rLength,1,MPI_INT,right,0,MPI_COMM_WORLD,&request0);
			MPI_Isend(&dataArray.at(rStart), rLength, MPI_dataArray, right, 0, MPI_COMM_WORLD, &request1);
                        }
			if(left!=right&&filesPerNode[myrank]-x>0){

				lStart=posIndex[left]+1;

				if(left==worldSize-1)
					lEnd=dataArray.size()-1;

				else 
					lEnd=posIndex[left+1];

				lLength=lEnd-lStart+1; 

				MPI_Isend(&lLength,1,MPI_INT,left,0,MPI_COMM_WORLD,&request2);
				MPI_Isend(&dataArray.at(lStart), lLength, MPI_dataArray, left, 0, MPI_COMM_WORLD, &request3);	
//			       cout<<myrank<<" sending "<<lLength<<" to "<<left << " " << lStart << " " <<lEnd <<endl;

			

			}
                        if(filesPerNode[right]-x>0){

			MPI_Recv(&rNewSize,1,MPI_INT,right,0,MPI_COMM_WORLD,&status0);

			tempDataR.reserve(rNewSize); 
			tempDataR.resize(rNewSize);

			MPI_Recv(&tempDataR.front(),rNewSize,MPI_dataArray,right,0,MPI_COMM_WORLD,&status1);
                        for(i=0;i<rNewSize;i++){
                        newData[right].push_back(tempDataR[i]);
                        }
			tempDataR.reserve(0); 
			tempDataR.resize(0);
    
                       } 
			if(left!=right&&filesPerNode[left]-x>0){

				MPI_Recv(&lNewSize,1,MPI_INT,left,0,MPI_COMM_WORLD,&status2);

  	  		        tempDataL.reserve(rNewSize); 
			        tempDataL.resize(rNewSize);

				MPI_Recv(&tempDataL.front(),lNewSize,MPI_dataArray,left,0,MPI_COMM_WORLD,&status3);
                                for(i=0;i<lNewSize;i++){
                        	newData[left].push_back(tempDataL[i]);
                                }
				tempDataL.reserve(0); 
				tempDataL.resize(0);

			}

                        if(filesPerNode[myrank]-x>0){
			MPI_Wait(&request0,&status0);
			MPI_Wait(&request1,&status1);
                        }
			if(left!=right&&filesPerNode[myrank]-x>0){

				MPI_Wait(&request2,&status2);
				MPI_Wait(&request3,&status3); 

			}

			x++;
			MPI_Barrier(MPI_COMM_WORLD);
//			cout<<endl<<endl;
			MPI_Barrier(MPI_COMM_WORLD);

		}

}

		// copy vector selection

                for(j=0;j<filesPerNode[myrank];j++){
		for (k = 0; k < posIndex[1]+1; k++)
		{
			newData[myrank].push_back(dataArray[j][k]);
		
		}
                }

		// clear old data

		dataArray.clear();
		dataArray.shrink_to_fit();

		// make new vector from smaller ones

		for (j = 0; j < worldSize; j++)
		{
			for (k = 0; k < newData[j].size(); k++)
			{
				dataArray[0].push_back(newData[j][k]);
			}
		}
}

		
