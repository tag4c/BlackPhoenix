#include <vector>
#include <stdlib.h>
#include "kdTree.h"
#include "sortPrep.h"
#include "math.h"
#include <iostream>
using namespace std;
void kdTree(vector<dataStruct> &data, int id, int left, int right, node *nodes){
  nodes[id].below=(right-left+1);
  //check if only one data point in node
  if(nodes[id].below==1){
    nodes[id].cent[0] = data[left].coordinates[0];
    nodes[id].cent[1] = data[left].coordinates[1];
    nodes[id].cent[2] = data[left].coordinates[2];
    nodes[id].length = 0;
    return;
  }
  double* min;
  double* max;
  min = new double[3];
  max = new double[3];
  for(int i=0;i<3;i++){
    min[i] = data[left].coordinates[i];
    max[i] = min[i];
  }
 
  //find min max
  for(int i=left+1;i<=right;i++){
    for(int j=0;j<3;j++){
      if(min[j]>data[i].coordinates[j]){
	min[j]=data[i].coordinates[j];
      }
      if(max[j]<data[i].coordinates[j]){
	max[j]=data[i].coordinates[j];
      }
    }
  }

  //find length of x y z
  double* temp = new double[3];
  temp[0]=max[0]-min[0];
  temp[1]=max[1]-min[1];
  temp[2]=max[2]-min[2];

  int dim=0;
  //find max length of x y or z
  if(temp[2]>temp[0]&&temp[2]>=temp[1])
    dim=2;
  if(temp[1]>temp[0]&&temp[1]>=temp[2])
    dim=1;

  //sort along that axis
  if(dim==0){
    std::sort(&data[left],&data[right],compareFunDim0);
  }
  else if(dim==1){
    std::sort(&data[left],&data[right],compareFunDim1);
  }
  else{
    std::sort(&data[left],&data[right],compareFunDim2);
  }
  
  //split array in half
  int mid=(right+left)/2;
  //find center points of x y z
  nodes[id].cent[0]=(min[0]+max[0])/2;
  nodes[id].cent[1]=(min[1]+max[1])/2;
  nodes[id].cent[2]=(min[2]+max[2])/2;
  
  //find the length for this node
  double tempC = 0;
  for(int i=0;i<3;i++){
    double tempFac = min[i] - nodes[id].cent[i];
    tempC += tempFac*tempFac; 
  }
  tempC = sqrt(tempC);
  nodes[id].length=tempC;

  delete min;
  delete max;
  delete temp;

  kdTree(data, id*2, left, mid, nodes);
  kdTree(data, id*2+1,mid+1, right, nodes);
}

void kdTree_search(const node* nodes, const double& radius, const double* sp, int layers,
		   vector< vector<int> >& searchListMtrx, long long int& count)
{
  int startNode = 1;
  int countPreList = 1;
  int countNowList;
  searchListMtrx[0][0]= startNode;
  int searchList = 0;
  int saveNewList;
  while(countPreList > 0){
    countNowList = 0;
    saveNewList = (searchList+1) % 2;
    for(int i=0;i<countPreList;i++){
      int nodeNum = searchListMtrx[searchList][i];
      double spToCent = dis(sp,nodes[nodeNum].cent);
      double maxNodeLen = nodes[nodeNum].length;
      if(radius >= spToCent + maxNodeLen){
	count += nodes[nodeNum].below;
	continue;
      }
      if(spToCent <= radius + maxNodeLen){
	searchListMtrx[saveNewList][countNowList] = 2*nodeNum;
	searchListMtrx[saveNewList][countNowList+1] = 2*nodeNum + 1; 
	countNowList += 2;
      }
    }
    countPreList = countNowList;
    searchList = saveNewList;
  }
}

double dis(const double* p1, const double* p2)
{ 
  double distance = 0;
  for(int i=0;i<3;i++){
    distance += (p1[i] - p2[i])*(p1[i] - p2[i]);
  }
  distance = sqrt(distance);
  return distance;
}

void verifySearch(vector<dataStruct>& data, double radius, double* sp){
  int count = 0;
  for(int i=0;i<data.size();i++){
    double p1[3];
    for(int j=0;j<3;j++){
      p1[j] = data[i].coordinates[j];
    }
    double distance = dis(p1,sp);
    if(distance < radius){
      count ++;
    }
  }
  cout << "the total points around = " << count << endl;
}
