/*
File:    writeData.h
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
#include <fstream>
#include <vector>
/*
==================================================
==================================================

Header Guards 

==================================================
*/
#ifndef DATA_H
#define DATA_H

struct dataStruct
{
	long int id;  // Line Number
	std::vector<double> coordinates;     // column 1
};



#endif

/*


Function Declarations

==================================================
*/

void writeFile(std::string &filepath, std::vector<dataStruct> &dataArray);

