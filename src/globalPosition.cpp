//the function recieves the local percentle position value and calculates for global value

#include "globalPosition.h"
#include <iostream>

void globalPositionValue (std::vector< std::vector <double> >& localPositionValueArray, int& numNodes, std::vector <double>& globalPositionValue, int &numOfBins)
{
  int i, j;
  double sumOfPos = double(0.0);
   //std::cout << "gpline10\n";
  for (j = 0; j < numNodes; j++)
  {
    sumOfPos += localPositionValueArray[j][0];

  }
  //std::cout << "gpline14\n";
  globalPositionValue.push_back(sumOfPos / numNodes);
  for (j = 1; j < (numOfBins); j++)
  {
    sumOfPos = 0;
    int count = 0;
    for (i = 0; i < numNodes; i++)
    {
      if (localPositionValueArray[i][j] > globalPositionValue[j - 1])
      {
        sumOfPos += localPositionValueArray[i][j];
        count++;
      }

    }
    globalPositionValue.push_back(sumOfPos / count);

  }

}
