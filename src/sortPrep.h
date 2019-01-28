#include <vector>
using namespace std;
#ifndef DATA_H 
#define DATA_H
struct dataStruct
{
long int id;
std::vector<double> coordinates;
};
#endif
void merge(vector<dataStruct> &a, dataStruct *tmp_array, int column, int left, int middle, int right ) ;

void sortPrep(vector<dataStruct> &a, int size, int column);
void sort(vector<dataStruct> &data, dataStruct *temp, int column,int left, int right);
