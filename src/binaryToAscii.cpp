/*
File:    binaryToAscii.h
Purpose: Read data from supplied file, output to raw binary file.
*/
/*
==================================================

Required Header Files

==================================================
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

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


    std::ifstream inFile("../datafiles/output/tag4ctest.bin", std::ios::binary | std::ios::in);
    

    linecount = 0;

    if (inFile.is_open())
    {
    	while ( inFile )
    	{
    		//inFile >> lineData.id;
    		inFile.read((char*)&lineData.id, 4);
    		inFile.read((char*)&lineData.col1, 8);
    		inFile.read((char*)&lineData.col2, 8);
    		inFile.read((char*)&lineData.col3, 8);
            //inFile >> lineData.col1;
            //inFile >> lineData.col2;
            //inFile >> lineData.col3;
            dataArray.push_back(lineData);

		}
		inFile.close();

		std::cout << dataArray[0].id << std::setprecision(15) << " " << dataArray[0].col1 << " " << dataArray[0].col2 << " " << dataArray[0].col3 << "\n";
/*
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

	    */

		

   	}
   	else
   	{
   		std::cout << "File wasn't opened.\n";
   	}



	return 0;
}