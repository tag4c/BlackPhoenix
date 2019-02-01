/*
File:    readData.cpp.
Purpose: Read binary data from supplied raw binary file, parse/pack data into appropriate structure for sorting
Input:   file (file handle), dataArray (pointer to array of structs)
Output:  dataArray (array of formatted structures)
Author:  Thomas Goff MTSU <tag4c@mtmail.mtsu.edu>
Usage:   readData(&file_handle, &dataArray)
*/

/*
==================================================

Required Header Files

==================================================
*/
#include <iostream>
#include <iomanip>
#include "readData.h"
/*
==================================================

Function Declarations

==================================================
*/

void readFile(std::string &filepath, std::vector<dataStruct> &dataArray)
{
	int i,j,k;
	//char a;
	//i = 0;
	
	dataStruct lineData;
	std::ifstream file (filepath.c_str());

	//std::string buff;
	//std::string index;
	//double col1, col2, col3;

	if (file.is_open())
	{
		while ( file )
		{

			file.read((char*)&lineData.id, 4);
    		file.read((char*)&lineData.coordinates[0], 8);
    		//lineData.coordinates.push_back(col1);
    		file.read((char*)&lineData.coordinates[1], 8);
    		//lineData.coordinates.push_back(col2);
    		file.read((char*)&lineData.coordinates[2], 8);
    		//lineData.coordinates.push_back(col3);
            dataArray.push_back(lineData);
            //lineData.coordinates.clear();
          // std::cout << dataArray[i].id << " " << std::setprecision(15) << dataArray[i].coordinates[0] << " " << dataArray[i].coordinates[1] << " " << dataArray[i].coordinates[2] << "\n";
          // std::cin >> a;
          // i++;

		}
		//std::cout << "File was read.\n";
		file.close();
      		dataArray.pop_back();
	}
    else
	{
		std::cout << "File wasn't opened...\n";
		exit(1);

	}

}

