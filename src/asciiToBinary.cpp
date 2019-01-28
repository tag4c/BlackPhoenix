/*
File:    asciiToBinary.h
Purpose: Read data from supplied file, output to raw binary file.
*/
/*
==================================================

Required Header Files

==================================================
*/
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <typeinfo>

/*
==================================================

Header Guards

==================================================
*/

#ifndef DATA_H
#define DATA_H

struct dataStruct
{
	int id;  // Line Number
	double col1;     // column 1
	double col2;
	double col3;
};


#endif


void read_directory(const std::string& name, std::vector<std::string>& v)
{
    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        v.push_back(dp->d_name);
    }
    closedir(dirp);
}
/*
==================================================

Main

==================================================
*/

int main()
{
    int i, j, k, linecount;
    int id;
    double col1, col2, col3;
    dataStruct lineData;
    std::vector<dataStruct> dataArray;
    std::vector<std::string> dirListing;
    std::string path = "../datafiles/ascii/input/";
    read_directory(path, dirListing);
    for (i = 0; i < dirListing.size(); i++)
    {
    	if (dirListing[i] == ".." || dirListing[i] == ".")
    	{
    		continue;

    	}
    	 	std::cout << dirListing[i] << std::endl;
    	
    }


    //std::cout << typeid(dirListing[i]).name() << std::endl;
   

    


  // std::ifstream inFile("../datafiles/input/datafile00001.txt");
    

    //linecount = 0;
/*
    if (inFile.is_open())
    {
    	while ( inFile )
    	{
    		inFile >> lineData.id;
            inFile >> lineData.col1;
            inFile >> lineData.col2;
            inFile >> lineData.col3;
            dataArray.push_back(lineData);

		}
		inFile.close();

		std::ofstream outFile("../datafiles/output/tag4ctest.bin", std::ios::binary | std::ios::out);
		if (outFile.is_open())
		{

			for (i = 0; i < dataArray.size(); i++)
			{
				//outFile << dataArray[i].id << dataArray[i].col1 << dataArray[i].col2 << dataArray[i].col3;
				//id = dataArray[i].id;
				outFile.write((char*)&dataArray[i].id, 4);
				outFile.write((char*)&dataArray[i].col1, 8);
				outFile.write((char*)&dataArray[i].col2, 8);
				outFile.write((char*)&dataArray[i].col3, 8);
			}

			outFile.close();
	    }
	    else
	    {
	    	std::cout << "\nOutput file wasn't opened properly.\n";
	    }

		

   	}
   	else
   	{
   		std::cout << "File wasn't opened.\n";
   	}
   	*/



	return 0;
}