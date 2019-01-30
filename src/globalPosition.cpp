//the function recieves the local percentle position value and calculates for global value

void globalPositionValue (vector< vector<double> >& localPositionValueArray, int& numNodes, vector<double>& globalPositionValue)
{
  int i,j;
  Double sumOfPos = Double(0.0);
  for(j=0; j<numNodes; j++)
    {
      sumOfPos+=localPositionValueArray[j][0];
      
    }
  globalPositionValue[0]=sumOfPos/numNodes;
  for (j=1; j<(numNodes-1); j++)
    {
      sumOfPos=0;
      count=0;
      for (i=0;i<numNodes;i++)
	{
	  if (localPositionValueArray[i][j]>globalPositionValue[j-1])
	    {
	      sumOfPos+=localPositionValueArray[i][j];
	      count++;
	    }
	  
	}
      globalPositionValue[j]=sumOfPos/count;

    }
  



}
