/*
File:    readData.h
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
	int id;  // Line Number
	double coordinates[3];     // column 1
};


#endif

/*
==================================================

Function Declarations

==================================================
*/

void readFile(std::string &filePath, std::vector<dataStruct> &dataArray, int &linesToRead);

