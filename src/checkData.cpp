#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main(int argc, char *argv[]){
	string name=argv[1];
	ifstream inFile;
	int index;
	double  one, two,i=1, d1,d2;
        cout<<name<<endl;	
	inFile.open(name);
	inFile>>index>>one>>d1>>d2;
	while(inFile){
	cout<<index<<endl;
	if(one>two)
	cout <<"line "<<i <<"out of order"<<endl;
	one=two;
	i++;
	inFile>>index>>two>>d1>>d2;
	}
	
return 0;
}
