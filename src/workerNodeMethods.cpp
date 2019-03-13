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

using namespace std;


void decodeFilesToRead(int &fileEachNodeSize, std::vector<int> &localFileList, std::vector <std::string> &fileList, std::string path)
{
	int i, j, k;
//	std::string fileName = "/data/shared/shared/coms7900-data/BlackPhoenixBinary/datafile";

		std::string fileName = path + "datafile";
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

void recvFilesToRead(int &fileNodeEachSize, MPI_Status &status, std::vector<int> &localFileList)
{
		MPI_Recv(&fileNodeEachSize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		//std::vector<int> localFileList;
		localFileList.reserve(fileNodeEachSize);
		localFileList.resize(fileNodeEachSize);

		MPI_Recv(&localFileList.front(), fileNodeEachSize, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
}

void sendLocalPercentile(int &worldSize, std::vector <double> &localPercentile, int &numOfBins) 
{
		MPI_Send(&localPercentile.front(), numOfBins-1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
}

void recvGlobalPositionValue(std::vector <double> &globalPositionValueData)
{
		int dataArraySize;
		MPI_Bcast(&dataArraySize, 1, MPI_INT, 0, MPI_COMM_WORLD);

		globalPositionValueData.reserve(dataArraySize);
		globalPositionValueData.resize(dataArraySize);
		MPI_Bcast(&globalPositionValueData.front(), dataArraySize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

void swapDataWorker(int &worldSize, std::vector <dataStruct> &dataArray, int &myrank,int *posIndex)
{
		int blockSize[2] = { 1, 3 };
	MPI_Datatype MPI_dataStruct[2] = { MPI_LONG_LONG_INT, MPI_DOUBLE };
	MPI_Datatype MPI_dataArray;
	MPI_Aint offsets[2];

	offsets[0] = offsetof(dataStruct, id);
	offsets[1] = offsetof(dataStruct, coordinates);

	MPI_Type_create_struct(2, blockSize, offsets, MPI_dataStruct, &MPI_dataArray);

	MPI_Type_commit(&MPI_dataArray); // tell MPI we're done constructing our data type
		int i, j, k, max=0;
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
                vector <dataStruct> tempDataR;
                vector <dataStruct> tempDataL;


                for(i=0;i<worldSize;i++){
                  if(max<filesPerNode[i])
                     max=filesPerNode[i];
                }
                int x=1; 

               for(k=0;k<=max;k++){
                x=0;
		while(x<=(wor(dSize)/2){

			right++;

			if(right>worldSize-1)
				right=0;

			left--;

			if(left<0)
				left=worldSize-1;

                        if(filesPerNode[myrank]-x>0){
			if(right==0){
				rStart=0; 
			}

			else{
				rStart=posIndex[right]+1;
			}

			if(right==worldSize-1)
				rEnd=dataArray.size()-1;

			else 
				rEnd=posIndex[right+1];

			if (right == 0)
			{
				rLength=rEnd-rStart+1;
			}
			else
			{
				rLength=rEnd-rStart+1;
			}


			MPI_Isend(&rLength,1,MPI_INT,right,0,MPI_COMM_WORLD,&request0);
			MPI_Isend(&dataArray.at(rStart), rLength, MPI_dataArray, right, 0, MPI_COMM_WORLD, &request1);
	//	       cout<<myrank<<" sending "<<rLength<<" to "<<right << " from " << rStart << " to  " <<rEnd <<endl;
                         }
			if(left!=right&&filesPerNode[myrank]-x>0){

				if(left==0){
					lStart=0; 

				}else
					lStart=posIndex[left]+1;



				if(left==worldSize-1)
					lEnd=dataArray.size()-1;

				else 
					lEnd=posIndex[left+1];

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
		                //cout<<myrank<<" sending "<<lLength<<" to "<<left << " from " << lStart << " to  " <<lEnd <<endl;
//			       cout<<myrank<<" sending "<<lLength<<" to "<<left << " " << lStart << " " <<lEnd <<endl;

			}
                        if(filesPerNode[right]-x>0){

                        MPI_Recv(&rNewSize,1,MPI_INT,right,0,MPI_COMM_WORLD,&status0);

                        tempDataR.reserve(rNewSize);
                        tempDataR.resize(rNewSize);

                        MPI_Recv(&tempDataR.front(),rNewSize,MPI_dataArray,right,0,MPI_COMM_WORLD,&status1);
                        for(i=0;i<rNewSize;i++){
                        newData[right].push(tempDataR[i]);
                        }
                        tempDataR.reserve(0);
                        tempDataR.resize(0);

                       }

			if(left!=rightfilesPerNode[left]-x>0){

                                MPI_Recv(&lNewSize,1,MPI_INT,left,0,MPI_COMM_WORLD,&status2);

                                tempDataL.reserve(rNewSize);
                                tempDataL.resize(rNewSize);

                                MPI_Recv(&tempDataL.front(),lNewSize,MPI_dataArray,left,0,MPI_COMM_WORLD,&status3);
                                for(i=0;i<lNewSize;i++){
                                newData[left].push(tempDataL[i]);
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

		int lastElement;

		if (myrank == worldSize -1)
		{
			lastElement = dataArray.size()-1;
		}
		else
		{
			lastElement = posIndex[myrank+1];
		}
                for(j=0;j<filesPerNode[myrank];j++){
		for (k = posIndex[myrank] + 1; k < lastElement+1; k++)
		{
			newData[myrank].push_back(dataArray[j][k]);
		}}

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
}
