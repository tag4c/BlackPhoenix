/*
File:    writeData.cpp
Depends: writeData.h
Purpose: Write binary data from supplied vector of type dataStruct, parse/unpack data from appropriate structure
Input:   file (file handle) ofstream, dataArray (pointer to vector of structs)
Output:  dataArray (array of formatted structures)
Author:  Thomas Goff MTSU <tag4c@mtmail.mtsu.edu>
Usage:   writeData(&file_handle, &dataArray)
Returns: 0 on success, 1 on failure
*/
/*
==================================================

Required Header Files

==================================================
*/
#include <iostream>
#include <iomanip>
#include "writeData.h"
/*
==================================================

Function Declarations

==================================================
*/

void writeFile(std::string &filepath, std::vector<dataStruct> &dataArray)
{
	int i,j,k;
	int linecount = 0;
        char a;

	dataStruct lineData;
	//std::vector <dataStruct> dataArray;

	std::string buff;
	std::string index;
	std::string col1;
	std::string col2;
	std::string col3;

	std::ofstream file(filepath);


	if (file.is_open())
	{
		while ( linecount < dataArray.size() )
		{

            file << linecount << ": "  << dataArray[linecount].id << " " << std::setprecision(15) << dataArray[linecount].coordinates[0] << " " << dataArray[linecount].coordinates[1] << " " << dataArray[linecount].coordinates[2] << "\n";
        //             std::cout << dataArray[linecount].id << " " << std::setprecision(15) << dataArray[linecount].coordinates[0] << " " << dataArray[linecount].coordinates[1] << " " << dataArray[linecount].coordinates[2] << "\n";
			linecount++;
		}

		file.close();


	}
    else
	{
		std::cout << "File wasn't opened...\n";
		exit(1);

	}
}


