#include <fstream>
#include <iostream>
#include "string"


using namespace std;
int main(){
string file1,file2,base1,base2,line;
int i,num;
ifstream inFile;
ofstream outFile;
base1="~/dtl2d/localstorage/public/coms7900-data/datafile00";
base2="./sample/datafile00";
num=1;
file1=base1+"00"+to_string(num)+".txt";
file2=base2+"00"+to_string(num)+".txt";

inFile.open(file1);
outFile.open(file2);
for(i=0;i<1000;i++){
getline(inFile,line);
outFile<<line;
}
inFile.close();
outFile.close();
return 0;
}

