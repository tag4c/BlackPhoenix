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
#include "sperateArray.h"

#define Int int
#define Double double



void sperateArray(std::vector<dataStruct>& dataArray, int& dataNum, std::vector<double>& globalPos, double numDataEachPart, int& columnToSort, std::vector<int>& posIndex)

{
	Int numofParts = globalPos.size(); // The number of parts we want to divide(Tips: minus 1, we ohly need the inside percentiles)
	// Start to find the position index
	for (int i = 0; i < numofParts; i++)
	{
		double globalPercentile = globalPos[i];
		int startPos = floor( (i + 1) * numDataEachPart );
		if (globalPercentile > dataArray[startPos].coordinates[columnToSort])
		{
			int currentPos = startPos + 1;
			while (globalPercentile > dataArray[currentPos].coordinates[columnToSort])
			{
				currentPos++;
			}
			posIndex.push_back(currentPos - 1);
		}
		else
		{
			int currentPos = startPos - 1;
			while (globalPercentile < dataArray[currentPos].coordinates[columnToSort])
			{
				currentPos--;
			}
			posIndex.push_back(currentPos);
		}
	}


}

