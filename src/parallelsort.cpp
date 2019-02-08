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




void read_directory(const std::string& name, std::vector<int>& v);
/* Main Routine */


int main(int argc, char *argv[])
{
	/* Variable Declarations */
	int i,j,
	k;
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
	


	/*  Create our MPI Datatype to pass our structure back and forth  */
	int blockSize[2] = { 1, 3 };
	MPI_Datatype MPI_dataStruct[2] = { MPI_INT, MPI_DOUBLE };
	MPI_Datatype MPI_dataArray;
	MPI_Aint offsets[2];

	offsets[0] = offsetof(dataStruct, id);
	offsets[1] = offsetof(dataStruct, coordinates);

	MPI_Type_create_struct(2, blockSize, offsets, MPI_dataStruct, &MPI_dataArray);

	MPI_Type_commit(&MPI_dataArray); // tell MPI we're done constructing our data type


	//std::cout << "My rank: " << myrank << std::endl;

	/* Scheduler Node (HEAD NODE) */
	if (myrank == 0)
	{
		/* Have this node read all data in and send it out first? */
		MPI_Request request;
		MPI_Status status;
		std::string dirpath = "../datafiles/binary/output/";
		std::vector <int> dirList;

		std::vector <std::vector<int>> fileEachNode;

		read_directory(dirpath, dirList);

		// divvy up the files between nodes.

		// need an array or vector to hold each nodes file list..
		

		//std::cout << worldSize << std::endl;

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
		


		int sentCount = 0;
    	//int fileEachNodeSize = fileEachNode[0].size();
    	//fileEachNodeSize *= fileEachNode.size();



    		// Bcast size of fileEachNode
    	//MPI_Bcast(&fileEachNode.front(), fileEachNode.size(), MPI_INT, 0, MPI_COMM_WORLD);
    	// <std::vector<std::string>> fileEachNode;dd
		int fileEachNodeSize = 0;

		for (i = 1; i < worldSize; i++)
		{
			fileEachNodeSize = fileEachNode[i].size();

			MPI_Isend(&fileEachNodeSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
			MPI_Isend(&fileEachNode[i].front(), fileEachNodeSize, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
		}

		fileEachNodeSize = fileEachNode[0].size();


		std::vector <std::string> fileList(fileEachNodeSize);

		std::string fileName = "../datafiles/binary/output/datafile";

		for (i = 0; i < fileEachNodeSize; i++)
		{
			std::string temp = to_string(fileEachNode[0][i]);
			while (temp.length() < 5)
			{
				temp = "0" + temp;
			}
			temp = temp + ".bin";
			fileList[i] = fileName + temp;
		}

		std::vector <dataStruct> dataArray;
		std::cout << myrank << " read " << fileList[0] << std::endl;
		
		readFile(fileList[0], dataArray);

		sortPrep(dataArray, 0);


		vector <double> globalPositionValueData;
		vector <vector <double>> localPercentileList;
		vector <double> localPercentile(worldSize-1);
		int numOfPercentiles = worldSize;
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

		globalPositionValue(localPercentileList, worldSize, globalPositionValueData);
		

		/* Broadcast gpv data to all nodes */

		arraySize = globalPositionValueData.size();
	//	std::cout << "Broadcasting gpv data to All nodes!" << std::endl;
		MPI_Bcast(&arraySize, 1, MPI_INT, 0, MPI_COMM_WORLD);
//		std::cout << "Broadcasting dataArray to all nodes!" << std::endl;
		MPI_Bcast(&globalPositionValueData.front(), arraySize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
//		std::cout << "Broadcasted to everyone!" << std::endl;

		vector <int> remotePosIndex;

		arraySize = dataArray.size();

		//double numDataEachPart;
		vector <int> posIndex;



		sperateArray(dataArray, arraySize, globalPositionValueData, numDataEachPart, columnToSort, posIndex);

		std::cout << "HEAD: " << posIndex.size() << std::endl;

		/*for(i=0;i<globalPositionValueData.size();i++)
		{
			std::cout << "globalPositionValueData[" << i << "] = " << globalPositionValueData[i] << endl;
		}*/
		for(i=0 ; i<posIndex.size(); i++)
		{
			std::cout << "HEAD: " << posIndex[i] << " ";
		//	std::cout << "HEAD: " << dataArray[posIndex[i]].coordinates[0] << " " << std::endl;
			//std::cout << "HEAD: " << dataArray[posIndex[i]+1].coordinates[0] << " " << std::endl;
		}
		std::cout << std::endl;
		

		// Comm protocol


		// pass localPercentileList to globalPosition

		MPI_Barrier(MPI_COMM_WORLD);

		int sentTotal = 0;

		MPI_Request request0;
		MPI_Request request1;
		MPI_Request request2;
		MPI_Request request3;

		MPI_Status  status0;
		MPI_Status  status1;
		MPI_Status  status2;
		MPI_Status  status3;
		int x=1; 
		int right=myrank;
		int left=myrank;
		int lNewSize,rNewSize;
		int rStart, lStart, rEnd, lEnd, rLength,lLength;
		vector <vector <dataStruct>> newData(worldSize);
                for(k=0;k<dataArray.size();k++){
                if(dataArray[k].coordinates[0]==.605941702998731)
                cout<<"I am node "<<myrank<<"and the odd index is at "<<k<<endl;
}
		while(x<=(worldSize)/2){

			right++;
			if(right>worldSize-1)
				right=0;
			left--;

			if(left<0)
				left=worldSize-1;

			rStart=posIndex[right-1]+1;

			if(right==worldSize-1)
				rEnd=dataArray.size()-1;

			else 
				rEnd=posIndex[right];

			rLength=rEnd-rStart+1; 

		       cout<<myrank<<" sending "<<rLength<<" to "<<right << " " << rStart << " " <<rEnd <<endl;

			MPI_Isend(&rLength,1,MPI_INT,right,0,MPI_COMM_WORLD,&request0);
			MPI_Isend(&dataArray.at(rStart), rLength, MPI_dataArray, right, 0, MPI_COMM_WORLD, &request1);
			sentTotal += rLength;

			if(left!=right){

				lStart=posIndex[left-1]+1;

				if(left==worldSize-1)
					lEnd=dataArray.size()-1;

				else 
					lEnd=posIndex[left];

				lLength=lEnd-lStart+1; 

				MPI_Isend(&lLength,1,MPI_INT,left,0,MPI_COMM_WORLD,&request2);
				MPI_Isend(&dataArray.at(lStart), lLength, MPI_dataArray, left, 0, MPI_COMM_WORLD, &request3);	
			       cout<<myrank<<" sending "<<lLength<<" to "<<left << " " << lStart << " " <<lEnd <<endl;

				sentTotal += lLength;

			}

			MPI_Recv(&rNewSize,1,MPI_INT,right,0,MPI_COMM_WORLD,&status0);

			newData[right].reserve(rNewSize); 
			newData[right].resize(rNewSize);

			MPI_Recv(&newData[right].front(),rNewSize,MPI_dataArray,right,0,MPI_COMM_WORLD,&status1);

			if(left!=right){

				MPI_Recv(&lNewSize,1,MPI_INT,left,0,MPI_COMM_WORLD,&status2);

				newData[left].reserve(lNewSize); 
				newData[left].resize(lNewSize);

				MPI_Recv(&newData[left].front(),lNewSize,MPI_dataArray,left,0,MPI_COMM_WORLD,&status3);

			}

			MPI_Wait(&request0,&status0);
			MPI_Wait(&request1,&status1);

			if(left!=right){

				MPI_Wait(&request2,&status2);
				MPI_Wait(&request3,&status3); 

			}

			x++;
			MPI_Barrier(MPI_COMM_WORLD);
			cout<<endl<<endl;
			MPI_Barrier(MPI_COMM_WORLD);

		}



		// copy vector selection

		for (k = 0; k < posIndex[0]+1; k++)
		{
			newData[myrank].push_back(dataArray[k]);
			sentTotal++;
		}

		int totalSize = 0;

		for (k = 0; k < worldSize; k++)
		{
			totalSize += newData[k].size();
			std::cout << myrank << " " << k << " " << newData[k].size() << std::endl;

		}
		std::cout << myrank << " total size sent: " << sentTotal << std::endl;

		std::cout << myrank << " total size recv: " << totalSize << std::endl;


		// clear old data

		dataArray.clear();
		dataArray.shrink_to_fit();

		// make new vector from smaller ones

		for (j = 0; j < worldSize; j++)
		{
			for (k = 0; k < newData[j].size(); k++)
			{
				dataArray.push_back(newData[j][k]);
			}
		}

		// sort

		sortPrep(dataArray, 0);

		int linecount = 0;
		std::string filepath = to_string(myrank) + "output.txt";
		std::ofstream file(filepath);

		//std::cout << "Size of dataArray: " << dataArray.size() << std::endl;
		// write new files

		if (file.is_open())
	{
		while ( linecount < dataArray.size() )
		{

            file << std::setprecision(15) << dataArray[linecount].coordinates[0] << "\n";
        //             std::cout << dataArray[linecount].id << " " << std::setprecision(15) << dataArray[linecount].coordinates[0] << " " << dataArray[linecount].coordinates[1] << " " << dataArray[linecount].coordinates[2] << "\n";
			linecount++;
		}

		file.close();


	}

		MPI_Barrier(MPI_COMM_WORLD);
	}
	/* Slave nodes (All others) */
	else
	{
		MPI_Request request;
		MPI_Status status;
		//int columnToSort;
		int fileNodeEachSize = 0;

		MPI_Recv(&fileNodeEachSize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		std::vector<int> localFileList;
		localFileList.reserve(fileNodeEachSize);
		localFileList.resize(fileNodeEachSize);

		MPI_Recv(&localFileList.front(), fileNodeEachSize, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		std::vector <std::string> fileList(fileNodeEachSize);

		std::string fileName = "../datafiles/binary/output/datafile";

		for (i = 0; i < fileNodeEachSize; i++)
		{
			std::string temp = to_string(localFileList[i]);
			while (temp.length() < 5)
			{
				temp = "0" + temp;
			}
			temp = temp + ".bin";
			fileList[i] = fileName + temp;


		}

		std::vector <dataStruct> dataArray;
		vector <double> globalPositionValueData;
		std::cout << myrank << " read " << fileList[0] << std::endl;
		readFile(fileList[0], dataArray);
		sortPrep(dataArray, 0);
	//	cout<<"my rank is "<<myrank<<" vector value is "<<dataArray[0].coordinates[0]<<endl;
		vector <double> localPercentile(worldSize-1);
		int numOfPercentiles = worldSize;
		int arraySize = dataArray.size();
		double numDataEachPart = 0.0;
		findPercentile(dataArray, numOfPercentiles, arraySize, columnToSort, localPercentile, numDataEachPart); 
		/* Send local percentile to head node */


		MPI_Send(&localPercentile.front(), worldSize - 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD); //, &request);

		int dataArraySize;
		MPI_Bcast(&dataArraySize, 1, MPI_INT, 0, MPI_COMM_WORLD);

		globalPositionValueData.reserve(dataArraySize);
		globalPositionValueData.resize(dataArraySize);
		MPI_Bcast(&globalPositionValueData.front(), dataArraySize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		arraySize = dataArray.size();

		vector <int> posIndex;

		sperateArray(dataArray, arraySize, globalPositionValueData, numDataEachPart, columnToSort, posIndex);

	   /* for(i=0;i<globalPositionValueData.size();i++)
		{
			std::cout << "globalPositionValueData[" << i << "] = " << globalPositionValueData[i] << endl;
		}*/
				for(i=0 ; i<posIndex.size(); i++)
		{
			std::cout << myrank << ": " << posIndex[i] << " ";
			//std::cout << myrank << ": " << dataArray[posIndex[i]].coordinates[0] << " " << std::endl;
			//std::cout << myrank << ": " << dataArray[posIndex[i]+1].coordinates[0] << " " << std::endl;
		}
		std::cout << std::endl;
		

			/* Send size of posIndex */

		MPI_Barrier(MPI_COMM_WORLD);

		MPI_Request request0;
		MPI_Request request1;
		MPI_Request request2;
		MPI_Request request3;
		MPI_Status  status0;
		MPI_Status  status1;
		MPI_Status  status2;
		MPI_Status  status3;
		int x=1; 
		int right=myrank;
		int left=myrank;
		int lNewSize,rNewSize;
		int rStart, lStart, rEnd, lEnd, rLength,lLength;
		vector <vector <dataStruct>> newData(worldSize);

		int sentTotal = 0;

                for(k=0;k<dataArray.size();k++){
                if(dataArray[k].coordinates[0]==.693684452093443)
                cout<<"I am node "<<myrank<<" and the odd index is at "<<k<<endl;
}
		while(x<=(worldSize)/2){

			right++;

			if(right>worldSize-1)
				right=0;

			left--;

			if(left<0)
				left=worldSize-1;

			if(right==0){
				rStart=0; 
			}

			else{
				rStart=posIndex[right-1]+1;
			}

			if(right==worldSize-1)
				rEnd=dataArray.size()-1;

			else 
				rEnd=posIndex[right];

			if (right == 0)
			{
				rLength=rEnd-rStart+1;
			}
			else
			{
				rLength=rEnd-rStart+1;
			}

			if(myrank==2){
			cout<<myrank<<" sending "<<rLength<<" to "<<right << " " << rStart << " " <<rEnd <<endl;
			}


			sentTotal += rLength;

			MPI_Isend(&rLength,1,MPI_INT,right,0,MPI_COMM_WORLD,&request0);
			MPI_Isend(&dataArray.at(rStart), rLength, MPI_dataArray, right, 0, MPI_COMM_WORLD, &request1);

			if(left!=right){

				if(left==0){
					lStart=0; 

				}else
					lStart=posIndex[left-1]+1;



				if(left==worldSize-1)
					lEnd=dataArray.size()-1;

				else 
					lEnd=posIndex[left];

				if (left == 0)
				{
					lLength=lEnd-lStart+1;
				}
				else
				{
					lLength=lEnd-lStart+1; 
				}

				MPI_Isend(&lLength,1,MPI_INT,left,0,MPI_COMM_WORLD,&request2);
				MPI_Isend(&dataArray.at(lStart), lLength, MPI_dataArray, left, 0, MPI_COMM_WORLD, &request3);
			       cout<<myrank<<" sending "<<lLength<<" to "<<left << " " << lStart << " " <<lEnd <<endl;

				sentTotal += lLength;

			}

			MPI_Recv(&rNewSize,1,MPI_INT,right,0,MPI_COMM_WORLD,&status0);

			newData[right].reserve(rNewSize); 
			newData[right].resize(rNewSize);

			MPI_Recv(&newData[right].front(),rNewSize,MPI_dataArray,right,0,MPI_COMM_WORLD,&status1);

			if(left!=right){

				MPI_Recv(&lNewSize,1,MPI_INT,left,0,MPI_COMM_WORLD,&status2);

				newData[left].reserve(lNewSize); 
				newData[left].resize(lNewSize);

				MPI_Recv(&newData[left].front(),lNewSize,MPI_dataArray,left,0,MPI_COMM_WORLD,&status3);

			}

			MPI_Wait(&request0,&status0);
			MPI_Wait(&request1,&status1);

			if(left!=right){
				MPI_Wait(&request2,&status2);
				MPI_Wait(&request3,&status3); 
			}

			x++;
			MPI_Barrier(MPI_COMM_WORLD);
			cout<<endl<<endl;
			MPI_Barrier(MPI_COMM_WORLD);
		}


		// copy vector selection

		int lastElement;

		if (myrank == worldSize -1)
		{
			lastElement = dataArray.size()-1;
		}
		else
		{
			lastElement = posIndex[myrank];
		}

		for (k = posIndex[myrank-1] + 1; k < lastElement+1; k++)
		{
			newData[myrank].push_back(dataArray[k]);
			sentTotal++;
		}



		int totalSize = 0;
		for (k = 0; k < worldSize; k++)
		{

			totalSize += newData[k].size();
			std::cout << myrank << " " << k << " " << newData[k].size() << std::endl;
		
		}
		std::cout << myrank << " total size sent: " << sentTotal << std::endl;
	std::cout << myrank << " total size recv: " << totalSize << std::endl;


		// clear old data

		dataArray.clear();
		dataArray.shrink_to_fit();

		// make new vector from smaller ones

		for (j = 0; j < worldSize; j++)
		{
			for (k = 0; k < newData[j].size(); k++)
			{
				dataArray.push_back(newData[j][k]);
			}
		}

		// sort

		sortPrep(dataArray, 0);


		// write new files

	/*	for (i = 0; i < 10; i++)
		{
			std::cout << "Node " << myrank << "Front of Array : " << dataArray[i].coordinates[0] << std::endl;
		}

		for (i = dataArray.size() - 10; i < dataArray.size(); i++)
		{
			std::cout << "Node " << myrank << "End of Array : " << dataArray[i].coordinates[0] << std::endl;
		} */
		std::string filepath = to_string(myrank) + "output.txt";
		std::ofstream file(filepath);
		int linecount = 0;

		//std::cout << "Size of dataArray: " << dataArray.size() << std::endl;


	if (file.is_open())
	{
		while ( linecount < dataArray.size() )
		{

            file << std::setprecision(15) << dataArray[linecount].coordinates[0] << "\n";
        //             std::cout << dataArray[linecount].id << " " << std::setprecision(15) << dataArray[linecount].coordinates[0] << " " << dataArray[linecount].coordinates[1] << " " << dataArray[linecount].coordinates[2] << "\n";
			linecount++;
		}

		file.close();


	}









		MPI_Barrier(MPI_COMM_WORLD);
	}

	MPI_Type_free(&MPI_dataArray); // clean up
	MPI_Finalize(); // clean up MPI usage


	return 0;


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




/* Main Routine function declarations  */ 


