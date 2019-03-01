#include <vector>
#include "sortPrep.h"
#include "math.h"
using namespace std;
#ifndef DATA_H 
#define DATA_H
struct dataStruct
{
  long long int id;
  double coordinates[3];//std::vector<double> coordinates;
};
#endif
#ifndef DATA_N 
#define DATA_N
struct node{
  int id;
  int below;
  double cent[3];
  double min[3];
  double max[3];
  double length; 
};
#endif
void kdTree(vector<dataStruct>& data,int id,int left, int right,node *nodes);
void kdTree_search(node* nodes, double radius, double* sp, int MaxNode, vector<int>& neighPoints, int nodeNum=1);
double dis(double* p1, double* p2);
