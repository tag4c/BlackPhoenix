// This function is used for find the percentile in a given array
// Input: dataArray, numPercentile, dataNum, columnToSort
// /* variable statement
//    dataArray     : The data array that we use
//    numPercentile : The number of parts we want to divide
//    dataNum       : The total number of the data array
// Output: vector<Double> percentiles, Double numDataEachPart
#include <vector>
#include <cmath>
#define Int int
#define Double double


#ifndef DATA_H
#define DATA_H

struct dataStruct
{
	int id;  // Line Number
	double coordinates[3];     // column 1
};


#endif

void findPercentile(std::vector<dataStruct>& dataArray, Int& numPercentile, Int& dataNum, Int& columnToSort, std::vector<Double>& percentiles, Double& numDataEachPart);
