// This function is used to sperate the data array by the global position
// Input  : dataNum, dataArray, globalPos, numDataEachPart
// /* variable statement
//    dataNum         : The size of the data array
//    dataArray       : The data array that we use
//    globalPos       : The global percentiles
//    numDataEachPart : The number of dataArray of each part
// */ End variable statement
// Output : posIndex
//    posIndex        : The index where to sperate the data array

#define Int int
#define Double double

void sperateArray(vector<dataStruct>& dataArray, Int& dataNum, vector<Double>& globalPos, Double numDataEachPart,  vector<Int>& posIndex)
{
  Int numofParts = globalPos.size(); // The number of parts we want to divide(Tips: minus 1, we ohly need the inside percentiles)
  // Start to find the position index
  for(Int i=0; i<numofParts; i++)
    {
      Double globalPercentile = globalPos[i];
      Int startPos = floor( (i+1) * numDataEachPart );
      if(globalPercentile > dataArray[startPos].x)
	{
	  Int currentPos = startPos + 1;
	  while(globalPercentile > dataArray[currentPos])
	    {
	      currentPos++;
	    }
	  PosIndex[i] = currentPos - 1;
	}
      else
	{
	  Int currentPos = startPos - 1;
	  while(globalPercentile < dataArray[currentPos])
	    {
	      currentPos--;
	    }
	  PosIndex[i] = currentPos + 1;
	}
    }
      
  
}
  
