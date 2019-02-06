#include <fstream>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <iomanip>
#include <unistd.h>
#include "mpi.h"

using namespace std;



int main(int argc, char *argv[])
{
	/* Variable Declarations */
	int i,j,k;
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
	alarm(10);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
	MPI_Request request;
	MPI_Status status;

/*
	/*  Create our MPI Datatype to pass our structure back and forth  
	int blockSize[2] = { 1, 3 };
	MPI_Datatype MPI_dataStruct[2] = { MPI_INT, MPI_DOUBLE };
	MPI_Datatype MPI_dataArray;
	MPI_Aint offsets[2];

	offsets[0] = offsetof(dataStruct, id);
	offsets[1] = offsetof(dataStruct, coordinates);

	MPI_Type_create_struct(2, blockSize, offsets, MPI_dataStruct, &MPI_dataArray);

	MPI_Type_commit(&MPI_dataArray); // tell MPI we're done constructing our data type

	std::cout << "My rank: " << myrank << std::endl;
*/
	/* Scheduler Node (HEAD NODE) */
	if (myrank == 0){
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
           int rStart, lStart, rEnd, lEnd, rLength,lLenth;
           vector <vector> <dataStruct> newData(worldSize);
           while(x<=(worldSize)/2){

           right++;
           if(right>worldSize-1)
           right=0;
           left--;
           if(left<0)
           left=worldSize-1;

            rStart=posIndex[right-1]+1;
          if(right==worldSize-1)
            rEnd=dataArray.size();
          else 
            rEnd=posIndex[right];
            rLength=rEnd-rStart; 
           MPI_Isend(&rLength,1,MPI_INT,right,0,MPI_COMM_WORLD,&request0);
	MPI_ISend(&dataArray[rStart], rLegth, MPI_dataArray, right, 0, MPI_COMM_WORLD, &request1);	
           if(left!=right){
            lStart=posIndex[left-1]+1;
          if(left==worldSize-1)
            lEnd=dataArray.size();
          else 
            lEnd=posIndex[left];
            lLength=lEnd-lStart; 
           MPI_Isend(&lLength,1,MPI_INT,left,0,MPI_COMM_WORLD,&request2);
	MPI_ISend(&dataArray[lStart], lLegth, MPI_dataArray, left, 0, MPI_COMM_WORLD, &request3);	
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
  
	}else{

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
           int rStart, lStart, rEnd, lEnd, rLength,lLenth;
           vector <vector> <dataStruct> newData(worldSize);
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
            rEnd=dataArray.size();
          else 
            rEnd=posIndex[right];
            rLength=rEnd-rStart; 
           MPI_Isend(&rLength,1,MPI_INT,right,0,MPI_COMM_WORLD,&request0);
	MPI_ISend(&dataArray[rStart], rLegth, MPI_dataArray, right, 0, MPI_COMM_WORLD, &request1);	
           if(left!=right){
             if(right==0){
                rStart=0; 
               }else
            lStart=posIndex[left-1]+1;
          if(left==worldSize-1)
            lEnd=dataArray.size();
          else 
            lEnd=posIndex[left];
            lLength=lEnd-lStart; 
           MPI_Isend(&lLength,1,MPI_INT,left,0,MPI_COMM_WORLD,&request2);
	MPI_ISend(&dataArray[lStart], lLegth, MPI_dataArray, left, 0, MPI_COMM_WORLD, &request3);	
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
          }


}
MPI_Finalize();
return 0;
}
