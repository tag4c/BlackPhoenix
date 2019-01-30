#include <vector>
using namespace std;
#ifndef DATA_H 
#define DATA_H
struct dataStruct
{
long int id;
double coordinates[3];//std::vector<double> coordinates;
};
#endif
void merge(vector<dataStruct> &data, dataStruct *tmp_array, int column, int left, int middle, int right ) ;

void sortPrep(vector<dataStruct> &data, int size, int column);
void sort(dataStruct *data, dataStruct *temp, int column,int left, int right);
