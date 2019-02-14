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
	long long int id;  // Line Number
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
    int i, j, k, fileCount;
    int id;
    double col1, col2, col3;
    dataStruct lineData;
    char a;
    
    std::vector<dataStruct> dataArray;
    std::vector<std::string> dirListing;
    std::vector<std::string> dataFileList;
    std::string path = "../../coms7900/";
    std::string outpath = "/data/shared/shared/coms7900-data/BlackPhoenixBinary/";
    std::string filepath; // reusable string to specify full string path
    std::cout << "dataArray size: " << dataArray.size() << std::endl;
    read_directory(path, dirListing);

    std::cout << "Finished reading directory." << std::endl;

    fileCount = 0;
    for (i = 0; i < dirListing.size(); i++)
    {
    	if (dirListing[i] == ".." || dirListing[i] == "." || dirListing[i] == "binary" || dirListing[i] == "BlackPhoenixBinary")
    	{
    		continue;

    	}
        else
        {
    	 	dataFileList.push_back(dirListing[i]);
            fileCount++;
        }
    	
    }

    std::cout << "Finished parsing file list" << std::endl;

    for (i = 0; i < 256; i++)
    {
        filepath = path + dataFileList[i];

        std::cout << "Reading: " << filepath << std::endl;

        std::ifstream inFile(filepath);
        if ( inFile.is_open() )
        {
            int lineCount = 0;
            while ( lineCount < 20000000 )
              //  while (lineCount < 1000000)
            {
                inFile >> lineData.id;
                inFile >> lineData.col1;
                inFile >> lineData.col2;
                inFile >> lineData.col3;
                dataArray.push_back(lineData);
                lineCount++;
    //            if(lineCount % 1000000 == 0){
     //           std::cout << lineCount << std::endl;
               // lineCount = lineCount-1000000;
      //          }
            }
            inFile.close();
            std::cout << "File: " << filepath << " lines: " << dataArray.size() << std::endl;
           // std::cout << dataArray[1001].id << " " << dataArray[1001].col1 << " " << std::endl;
            //std::cin >> a;

        }
        else
        {
            std::cout << "\nInput file wasn't opened properly.\n";
            std::cout << filepath << std::endl;
            exit(1);
        }
        filepath = outpath + dataFileList[i].substr(0, dataFileList[i].size()-4) + ".bin";
        std::ofstream outFile(filepath, std::ios::binary | std::ios::out);
        std::cout << "Writing new file: " << filepath << std::endl;
        if (outFile.is_open())
        {

            for (j = 0; j < dataArray.size(); j++)
            {
                outFile.write((char*)&dataArray[j].id, 8);
                outFile.write((char*)&dataArray[j].col1, 8);
                outFile.write((char*)&dataArray[j].col2, 8);
                outFile.write((char*)&dataArray[j].col3, 8);
            }

            outFile.close();
        }
        else
        {
            std::cout << "\nOutput file wasn't opened properly.\n";
            exit(1);
        }

        std::cout << "Done writing file." << std::endl;

        dataArray.clear();


        
    }

	return 0;
}
