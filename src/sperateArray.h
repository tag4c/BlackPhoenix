// This function is used to sperate the data array by the global position
// Input  : dataNum, dataArray, globalPos, numDataEachPart, columnToSort
// /* variable statement
//    dataNum         : The size of the data array
//    dataArray       : The data array that we use
//    globalPos       : The global percentiles
//    numDataEachPart : The number of dataArray of each part
// */ End variable statement
// Output : posIndex
//    posIndex        : The index where to sperate the data array
#include <vector>
#include <cmath>

#ifndef DATA_H
#define DATA_H

struct dataStruct
{
	int id;  // Line Number
	double coordinates[3];     // column 1
};
#endif

void sperateArray(std::vector<dataStruct>& dataArray, int& dataNum, std::vector<double>& globalPos, double numDataEachPart, int& columnToSort, std::vector<int>& posIndex);
  
