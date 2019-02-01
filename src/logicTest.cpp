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
           MPI_Request request1;
           MPI_Request request2;
           MPI_Status  status1;
           MPI_Status  status2;
           int x=1; 
           int right=myrank;
           int left=myrank;
           int doko1,doko2;
           while(x<worldSize-1){

           right++;
           if(right>worldSize-1)
           right=0;
           left--;
           if(left<0)
           left=worldSize-1;


           MPI_Isend(&myrank,1,MPI_INT,right,0,MPI_COMM_WORLD,&request1);
           MPI_Isend(&myrank,1,MPI_INT,left,0,MPI_COMM_WORLD,&request2);
           MPI_Recv(&doko1,1,MPI_INT,right,0,MPI_COMM_WORLD,&status);
           MPI_Recv(&doko2,1,MPI_INT,left,0,MPI_COMM_WORLD,&status);
           MPI_Wait(&request1,&status1);
           MPI_Wait(&request2,&status2); 
           cout<<"x="<<x<<" "<<myrank<<"<->"<<doko1<< " right" << endl;
           cout<<"x="<<x<<" "<<myrank<<"<->"<<doko2<< " left" << endl;
           x++;
           MPI_Barrier(MPI_COMM_WORLD);
           cout<<endl<<endl;
           MPI_Barrier(MPI_COMM_WORLD);
           }
  
	}else{

           MPI_Request request1;
           MPI_Request request2;
           MPI_Status  status1;
           MPI_Status  status2;

           int x=1; 
           int right=myrank;
           int left=myrank;
           int doko1, doko2; 
           bool first;
           if(myrank%2==0)
           first=true;
           else
           first=false;
           while(x<worldSize-1){
           right++;
           if(right>worldSize-1)
           right=0;
           left--;
           if(left<0)
           left=worldSize-1;

           MPI_Isend(&myrank,1,MPI_INT,right,0,MPI_COMM_WORLD,&request1);
           MPI_Isend(&myrank,1,MPI_INT,left,0,MPI_COMM_WORLD,&request2);
           MPI_Recv(&doko1,1,MPI_INT,right,0,MPI_COMM_WORLD,&status);
           MPI_Recv(&doko2,1,MPI_INT,left,0,MPI_COMM_WORLD,&status);
           MPI_Wait(&request1,&status1); 
           MPI_Wait(&request2,&status2); 
         //  cout<<"x="<<x<<" "<<myrank<<"<->"<<doko1<< " right" << endl;
          // cout<<"x="<<x<<" "<<myrank<<"<->"<<doko2<< " left" << endl;
           x++;
           MPI_Barrier(MPI_COMM_WORLD);
           MPI_Barrier(MPI_COMM_WORLD);

          /* 
           if(first){
           if(myrank<right){
           MPI_Send(&myrank,1,MPI_INT,right,0,MPI_COMM_WORLD);
           MPI_Recv(&doko,1,MPI_INT,right,0,MPI_COMM_WORLD,&status);
           }
           else{
           MPI_Recv(&doko,1,MPI_INT,right,0,MPI_COMM_WORLD,&status);
           MPI_Send(&myrank,1,MPI_INT,right,0,MPI_COMM_WORLD);
           }
           cout<<"x="<<x<<" "<<myrank<<"<->"<<doko<< " first" << endl;
           }
           else if(!first){
           if(myrank<left){
           MPI_Send(&myrank,1,MPI_INT,left,0,MPI_COMM_WORLD);
           MPI_Recv(&doko,1,MPI_INT,left,0,MPI_COMM_WORLD,&status);
           }
           else{
           MPI_Recv(&doko,1,MPI_INT,left,0,MPI_COMM_WORLD,&status);
           MPI_Send(&myrank,1,MPI_INT,left,0,MPI_COMM_WORLD);
           }
           cout<<"x="<<x<<" "<<myrank<<"<->"<<doko<< " first" << endl;
           }
           if(right==left)
           break;

           MPI_Barrier(MPI_COMM_WORLD);
           if(first){
           if(myrank<left){
           MPI_Send(&myrank,1,MPI_INT,left,0,MPI_COMM_WORLD);
           MPI_Recv(&doko,1,MPI_INT,left,0,MPI_COMM_WORLD,&status);
           }
           else{
           MPI_Recv(&doko,1,MPI_INT,left,0,MPI_COMM_WORLD,&status);
           MPI_Send(&myrank,1,MPI_INT,left,0,MPI_COMM_WORLD);
           }
           cout<<"x="<<x<<" "<<myrank<<"<->"<<doko<< " second" << endl;
           }
           else if(!first){
           if(myrank<right){
           MPI_Send(&myrank,1,MPI_INT,right,0,MPI_COMM_WORLD);
           MPI_Recv(&doko,1,MPI_INT,right,0,MPI_COMM_WORLD,&status);
           }
           else{
           MPI_Recv(&doko,1,MPI_INT,right,0,MPI_COMM_WORLD,&status);
           MPI_Send(&myrank,1,MPI_INT,right,0,MPI_COMM_WORLD);
           }
           cout<<"x="<<x<<" "<<myrank<<"<->"<<doko<< " second" << endl;
           }i*/


}
}
MPI_Finalize();
return 0;
}
