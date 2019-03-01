#include <vector>
#include <stdlib.h>
#include "kdTree.h"
#include "sortPrep.h"
#include "math.h"
#include <iostream>
using namespace std;
void kdTree(vector<dataStruct> &data, int id, int left, int right, node *nodes){
  int i,j,k;
  int dim;
  double temp[3];
  nodes[id].id = id;
  cout << "id  = " << id << endl;
  nodes[id].below=(right-left+1);
  cout << "below = " << nodes[id].below << endl;
  //check if only one data point in node
  if(nodes[id].below==1){
    return;}
  nodes[id].min[0]=data[left].coordinates[0];
  nodes[id].min[1]=data[left].coordinates[1];
  nodes[id].min[2]=data[left].coordinates[2];
  nodes[id].max[0]=data[left].coordinates[0];
  nodes[id].max[1]=data[left].coordinates[1];
  nodes[id].max[2]=data[left].coordinates[2];
  cout << "start of find min and max" << endl;
  //find min max
  for(i=left;i<=right;i++){
    if(nodes[id].min[0]>data[i].coordinates[0]){
      nodes[id].min[0]=data[i].coordinates[0];
    }
    if(nodes[id].min[1]>data[i].coordinates[1]){
      nodes[id].min[1]=data[i].coordinates[1];
    }
    if(nodes[id].min[2]>data[i].coordinates[2]){
      nodes[id].min[2]=data[i].coordinates[2];
    }
    if(nodes[id].max[0]<data[i].coordinates[0]){
      nodes[id].max[0]=data[i].coordinates[0];
    }
    if(nodes[id].max[1]<data[i].coordinates[1]){
      nodes[id].max[1]=data[i].coordinates[1];
    }
    if(nodes[id].max[2]<data[i].coordinates[2]){
      nodes[id].max[2]=data[i].coordinates[2];
    }
  }
  cout << "end of find min and max" << endl;
  //find length of x y z
  temp[0]=nodes[id].max[0]-nodes[id].min[0];
  temp[1]=nodes[id].max[1]-nodes[id].min[1];
  temp[2]=nodes[id].max[2]-nodes[id].min[2];
  dim=0;
  cout << "start of find the leng" << endl;
  //find max length of x y or z
  if(temp[2]>temp[0]&&temp[2]>=temp[1])
    dim=2;
  if(temp[1]>temp[0]&&temp[1]>=temp[2])
    dim=1;
  cout << "end of find len" << endl;
  //sort along that axis
  sortPrep(data,dim,left,right);
  cout << "end of sort" << endl;
  //split array in half
  int mid=(right+left)/2;
  //find center points of x y z
  nodes[id].cent[0]=(nodes[id].min[0]+nodes[id].max[0])/2;
  nodes[id].cent[1]=(nodes[id].min[1]+nodes[id].max[1])/2;
  nodes[id].cent[2]=(nodes[id].min[2]+nodes[id].max[2])/2;
  double tempC[8];
  //make corners
  tempC[0]=sqrt(pow(nodes[i].min[0]-nodes[i].cent[0],2)+pow(nodes[i].min[1]-nodes[i].cent[1],2)+pow(nodes[i].min[2]-nodes[i].cent[2],2));
  tempC[1]=sqrt(pow(nodes[i].min[0]-nodes[i].cent[0],2)+pow(nodes[i].min[1]-nodes[i].cent[1],2)+pow(nodes[i].max[2]-nodes[i].cent[2],2));
  tempC[2]=sqrt(pow(nodes[i].min[0]-nodes[i].cent[0],2)+pow(nodes[i].max[1]-nodes[i].cent[1],2)+pow(nodes[i].min[2]-nodes[i].cent[2],2));
  tempC[3]=sqrt(pow(nodes[i].min[0]-nodes[i].cent[0],2)+pow(nodes[i].max[1]-nodes[i].cent[1],2)+pow(nodes[i].max[2]-nodes[i].cent[2],2));
  tempC[4]=sqrt(pow(nodes[i].max[0]-nodes[i].cent[0],2)+pow(nodes[i].min[1]-nodes[i].cent[1],2)+pow(nodes[i].min[2]-nodes[i].cent[2],2));
  tempC[5]=sqrt(pow(nodes[i].max[0]-nodes[i].cent[0],2)+pow(nodes[i].min[1]-nodes[i].cent[1],2)+pow(nodes[i].max[2]-nodes[i].cent[2],2));
  tempC[6]=sqrt(pow(nodes[i].max[0]-nodes[i].cent[0],2)+pow(nodes[i].max[1]-nodes[i].cent[1],2)+pow(nodes[i].min[2]-nodes[i].cent[2],2));
  tempC[7]=sqrt(pow(nodes[i].max[0]-nodes[i].cent[0],2)+pow(nodes[i].max[1]-nodes[i].cent[1],2)+pow(nodes[i].max[2]-nodes[i].cent[2],2));
  nodes[id].length=tempC[0];
  
  //find distance to farthest corner
  for(i=1;i<8;i++){
    if(tempC[i]>nodes[id].length)
      nodes[id].length=tempC[i];
  }
  kdTree(data, id*2, left, mid, nodes);
  kdTree(data, id*2+1,mid+1, right, nodes);
}

void kdTree_search(node* nodes, double radius, double* sp, int maxNode, vector<int>& neighPoints, int nodeNum)
{
  double spToCent = dis(sp,nodes[nodeNum].cent);
  double maxNodeLen = nodes[nodeNum].length;
  if(spToCent < radius + maxNodeLen){
    int subNodeNum1 = nodeNum*2;
    if(subNodeNum1 > maxNode){
      exit(1);
    }
    kdTree_search(nodes,radius,sp,subNodeNum1);
    int subNodeNum2 = nodeNum*2+1;
    if(subNodeNum2 > maxNode){
      exit(1);
    }
    kdTree_search(nodes,radius,sp,subNodeNum2);
  }
  else{
    neighPoints.push_back(nodes[nodeNum].id);
  }
}  

double dis(double* p1, double* p2)
{ 
  double distance = 0;
  for(int i=0;i<3;i++){
    distance += pow(p1[i] - p2[i], 2);
  }
  distance = sqrt(distance);
  return distance;
}
