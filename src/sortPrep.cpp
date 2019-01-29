#include<vector>
#include <stdlib.h>
#include "sortPrep.h"
using namespace std;
void sortPrep(vector<dataStruct> &a,int size,int column){
	//temp array to swap data
	dataStruct *tmp;
	tmp =(dataStruct *) malloc((size)*sizeof(dataStruct));
	//begin recursive sort
	sort(a,tmp,column,0,size-1);

}
 
void sort(vector<dataStruct> &data, dataStruct *tmp,int column,int left, int right){
	long middle=(left+right)/2;
	//dividing out for merge
	if(left<right){
	sort(data,tmp,column,left,middle);
	sort(data,tmp,column,middle+1,right);
        //merge call 
	merge(data,tmp,column,left,middle+1,right);
   }
}

void merge(vector<dataStruct> &a, dataStruct *tmp,int column, int left, int middle, int right ) {

    int i, left_end, num_elements, tmp_pos;
    left_end = middle-1;
    tmp_pos = left;
    num_elements = right-left+1;
    while((left <= left_end) && (middle <= right))
        if(a[left].coordinates[column] <= a[middle].coordinates[column])
            tmp[tmp_pos++] = a[left++];
        else
            tmp[tmp_pos++] = a[middle++];
    while(left <= left_end)  
        tmp[tmp_pos++] = a[left++];
    while(middle <= right) 
        tmp[tmp_pos++] = a[middle++];
    for(i=1; i <= num_elements; i++, right-- )
        a[right] = tmp[right];

}
