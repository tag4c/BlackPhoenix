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

void sperateArray(vector<dataStruct>& dataArray, Int& dataNum, vector<Double>& globalPos, Double numDataEachPart, Int& columnToSort, vector<Int>& posIndex);
  
