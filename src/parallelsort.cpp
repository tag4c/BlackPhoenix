/* Black Phoenix Parallel Sort main routine    */
// Some group information here...


/* Header files */

#include <fstream>
#include <iostream>
#include <cmath>
#include "string.h"
#include <vector>
#include "sortPrep.h"
using namespace std;
//#include "mpi.h"

// Common library includes for everyone...

// Specific includes for individually authored code


// Functions needed for header includes
/*
read in data
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
// Function prototypes    */
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
	long int id;  // Line Number
	std::vector<double> coordinates;     // column 1
};
#endif




/* Main Routine */
int main(int argc, char *argv[])
{
	/* Variable Declarations */
	int i,j,k;
	int maxFiles;
	int maxNodes;
	int columnToSort;


	/* Variable initialization */
	maxFiles = atoi(argv[1]);  // First command line argument for Number of files to read
	maxNodes = atoi(argv[2]); // Second command line argument for number of compute nodes to use for the sort
	columnToSort = atoi(argv[3]); // third command line argument - column  to sort on
       const  int size=20000000;
	/* Variable initialization */
//	maxFiles = atoi(argv[1]);  // First command line argument for Number of files to read
//	maxNodes = atoi(argv[2]); // Second command line argument for number of compute nodes to use for the sort

	/* MPI Setup */
//	int myrank, worldSize; // myrank - Node ID, worldSize - number of nodes available
//	MPI_Request req;
//	MPI_Status status;

       // dataStruct *data;
        vector<dataStruct> data;
	//data =(dataStruct *) malloc((size)*sizeof(dataStruct));
        ifstream inFile;
        ofstream outFile;
        inFile.open("inData.txt");
        outFile.open("outData.txt");

        double val;
        for(int i=0;i<size;i++){
        data.push_back(dataStruct());
        inFile>>data[i].id;
        inFile>>val;
        data[i].coordinates.push_back(val);
        inFile>>val;
        data[i].coordinates.push_back(val);
        inFile>>val;
        data[i].coordinates.push_back(val);
         }
 
        sortPrep(data,size,columnToSort);
        for(int i=0;i<size;i++){
        outFile<<data[i].id<<"	"<<data[i].coordinates[0]<<"	"
        <<data[i].coordinates[1]<<"	"<<data[i].coordinates[2]<<endl;
         }



}




/* Main Routine function declarations  */ 

