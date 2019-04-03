#include<vector>
#include <stdlib.h>
#include "sortPrep.h"
using namespace std;
void sortPrep(vector<dataStruct> &data,int column, int left, int right){
	//temp array to swap data
	int size = data.size();
	dataStruct *tmp;
	tmp =(dataStruct *) malloc((size)*sizeof(dataStruct));
	//begin recursive sort
	sort1(data,tmp,column,left,right);
	free(tmp);
}
 
void sort1(vector<dataStruct> &data, dataStruct *tmp,int column,int left, int right){
	long middle=(left+right)/2;
	//dividing out for merge
	if(left<right){
	sort1(data,tmp,column,left,middle);
	sort1(data,tmp,column,middle+1,right);
        //merge call 
	merge(data,tmp,column,left,middle+1,right);
   }
}

void merge(vector<dataStruct> &data, dataStruct *tmp,int column, int left, int middle, int right ) {

    int i, left_end, num_elements, tmp_pos;
    left_end = middle-1;
    tmp_pos = left;
    num_elements = right-left+1;
    while((left <= left_end) && (middle <= right))
        if(data[left].coordinates[column] <= data[middle].coordinates[column])
            tmp[tmp_pos++] = data[left++];
        else
            tmp[tmp_pos++] = data[middle++];
    while(left <= left_end)  
        tmp[tmp_pos++] = data[left++];
    while(middle <= right) 
        tmp[tmp_pos++] = data[middle++];
    for(i=1; i <= num_elements; i++, right-- )
        data[right] = tmp[right];

}


bool compareFunDim0(const dataStruct& a, const dataStruct& b){
  return a.coordinates[0] < b.coordinates[0];
}

bool compareFunDim1(const dataStruct& a, const dataStruct& b){
  return a.coordinates[1] < b.coordinates[1];
}

bool compareFunDim2(const dataStruct& a, const dataStruct& b){
  return a.coordinates[2] < b.coordinates[2];
}

