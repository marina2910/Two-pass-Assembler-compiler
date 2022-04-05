#include <fstream>
#include <iostream>
#include <string.h>
#include "../inc/Assembler.hpp"

int main(int argc, char *argv[])
{

    std::string inputFileName;
    std::string outputFileName;
    if (argc == 2) //No output file name is defined
    {
        inputFileName = argv[1];
        outputFileName = "outputFile";
    }
    else if (argc == 4 && strcmp(argv[1], "-o") == 0) //Output file name is passed as parameter
    {
        inputFileName = argv[3];
        outputFileName = argv[2];
    }
    else //Invalid arguments
    {
        std::cerr << "Error: invalid arguments! " << std::endl;
        return -1;
    }

    //OPENING INPUT AND OUTPUT FILES
    std::ifstream inputFile(inputFileName);
    std::ofstream outputFile(outputFileName);
    if (!inputFile.is_open())
    {
        std::cerr << "Error: unable to open input file!" << std::endl;
        return -1;
    }
    if (!outputFile.is_open())
    {
        std::cerr << "Error: unable to open output file!" << std::endl;
        inputFile.close();
        return -1;
    }
    //ASSEMBLER CALLED
    Assembler assembler(&inputFile, &outputFile);
    assembler.firtsPass();
        fflush(stdout);

    assembler.secondPass();
        fflush(stdout);

    //CLOSING INPUT AND OUTPUT FILES
    inputFile.close();
    outputFile.close();
    fflush(stdout);

}