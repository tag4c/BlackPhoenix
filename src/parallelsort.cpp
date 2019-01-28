/* Black Phoenix Parallel Sort main routine    */
// Some group information here...


/* Header files */

#include <fstream>
#include <iostream>
#include <cmath>
#include "string.h"
#include <vector>
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
struct dataStruct
{
	long int id;  // Line Number
	double x;     // column 1
	double y;     // column 2
	double z;     // column 3
};
void sort(vector<dataStruct> &data, dataStruct *temp, int column,int left, int right);
void sortPrep(vector<dataStruct> &a, int size);
void merge(vector<dataStruct> &a, dataStruct *tmp_array, int column, int left, int middle, int right ) ;
// Common functions everyone will use



/* Data structure setup */
/*
Overall Data List is an array of structures of this format:
structure format:

long integer   |   double   |   double  |   double

*/





/* Main Routine */
int main(int argc, char *argv[])
{
	/* Variable Declarations */
	int i,j,k;
	int maxFiles;
	int maxNodes;
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


        for(int i=0;i<size;i++){
        data.push_back(dataStruct());
        inFile>>data[i].id;
        inFile>>data[i].x;
        inFile>>data[i].y;
        inFile>>data[i].z;
         }
 
        sortPrep(data,size);
        for(int i=0;i<size;i++){
        outFile<<data[i].id<<"	"<<data[i].x<<"	"<<data[i].y<<"	"<<data[i].z<<endl;
         }



}




/* Main Routine function declarations  */ 

void sortPrep(vector<dataStruct> &a,int size){
	//temp array to swap data
	dataStruct *tmp;
	tmp =(dataStruct *) malloc((size)*sizeof(dataStruct));
	//begin recursive sort
	sort(a,tmp,1,0,size-1);

}
 
void sort(vector<dataStruct> &data, dataStruct *tmp,int column,int left, int right){
	long middle=(left+right)/2;
	//dividing out for merge
	if(left<right){
	sort(data,tmp,column,left,middle);
	sort(data,tmp,column,middle+1,right);
        //merge call 
	merge(data,tmp,column,left,middle+1,right);
   }
}

void merge(vector<dataStruct> &a, dataStruct *tmp,int column, int left, int middle, int right ) {

    int i, left_end, num_elements, tmp_pos;
    left_end = middle-1;
    tmp_pos = left;
    num_elements = right-left+1;
    while((left <= left_end) && (middle <= right))
        if(a[left].x <= a[middle].x)
            tmp[tmp_pos++] = a[left++];
        else
            tmp[tmp_pos++] = a[middle++];
    while(left <= left_end)  
        tmp[tmp_pos++] = a[left++];
    while(middle <= right) 
        tmp[tmp_pos++] = a[middle++];
    for(i=1; i <= num_elements; i++, right-- )
        a[right] = tmp[right];

}
