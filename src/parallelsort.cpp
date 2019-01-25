/* Black Phoenix Parallel Sort main routine    */
// Some group information here...


/* Header files */

#include <fstream>
#include <iostream>
#include <cmath>
#include "string.h"
#include "mpi.h"

// Common library includes for everyone...

// Specific includes for individually authored code


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

struct dataStruct
{
	long int id;  // Line Number
	double x;     // column 1
	double y;     // column 2
	double z;     // column 3
};



/* Main Routine */

int main(int argc, char *argv[])
{
	/* Variable Declarations */
	int i,j,k;
	int maxFiles;
	int maxNodes;


	/* Variable initialization */
	maxFiles = atoi(argv[1]);  // First command line argument for Number of files to read
	maxNodes = atoi(argv[2]); // Second command line argument for number of compute nodes to use for the sort

	/* MPI Setup */
	int myrank, worldSize; // myrank - Node ID, worldSize - number of nodes available
	MPI_Request req;
	MPI_Status status;



}




/* Main Routine function declarations  */ 


