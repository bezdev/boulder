#include <vector>
#include <fstream>

#include "FileReader.h"

#ifdef DEBUG
    static std::string OBJ_PATH("Debug\\");
#else
    static std::string OBJ_PATH("Release\\");
#endif

std::vector<char> FileReader::ReadCSO(const char * filename)
{
    std::ifstream fin(OBJ_PATH + filename, std::ios::binary);
    fin.seekg(0, std::ios_base::end);
    int size = (int)fin.tellg();
    fin.seekg(0, std::ios_base::beg);
    std::vector<char> csoData(size);

    fin.read(&csoData[0], size);
    fin.close();

    return csoData;
}

std::vector<char> FileReader::ReadHLSL(const char * filename)
{
    std::ifstream fin(filename, std::ios::binary);
    fin.seekg(0, std::ios_base::end);
    int size = (int)fin.tellg();
    fin.seekg(0, std::ios_base::beg);
    std::vector<char> csoData(size);

    fin.read(&csoData[0], size);
    fin.close();

    return csoData;
}