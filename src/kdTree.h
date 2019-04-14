#include <vector>
#include "sortPrep.h"
#include "math.h"
#include <algorithm>
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
int below;
double cent[3];
double length; 
};
#endif
void kdTree(vector<dataStruct>& data, int id,int left, int right,node *nodes);
void kdTree_search(const node* nodes, const double& radius, const double* sp, int layers,vector< vector<int> >& searchListMtrx, long long int& count);
double dis(const double* p1, const double* p2);
void verifySearch(vector<dataStruct>& data, double radius, double* sp);
