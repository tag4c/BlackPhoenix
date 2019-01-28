// This function is used for find the percentile in a given array
// Input: dataArray, numPercentile, dataNum, columnToSort
// /* variable statement
//    dataArray     : The data array that we use
//    numPercentile : The number of parts we want to divide
//    dataNum       : The size of the data array
// */ End variable statement
#define Int int
#define Double double

void findPercentile(vector<dataStruct>& dataArray, Int& numPercentile, Int& dataNum, Int& columnToSort, vector<Double>& percentiles, Double& numDataEachPart)
{
  Int position, i;
  // position : The positon of dataArray
  // i        : For the loop index
  Double ratio;
  // ratio    : The ratio to calculate the percentile

  // PercentEachPart    : The percent of each part
  // numDataCurrentPart : The number of dataArray of each part
  Double percentEachPart = Double(1.0) / numPercentile;
  Double numDataEachPart = percentEachPart * dataNum;

  // Calculate the percentiles
  for(i=1;i<numPercentile;i++)
    {
      Double numDataCurrentPart = numDataEachPart * i;
      position = floor(numDataCurrentPart);
      ratio = numDataCurrentPart - position;
      percentiles[i-1] = dataArray[position].coordinates[columnToSort] * (1-ratio) + dataArray[position].coordinates[columnToSort] * ratio;
    }      

}
   
