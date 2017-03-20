#pragma once

class FileReader
{
public:
    static std::vector<char> FileReader::ReadCSO(const char * filename);
    static std::vector<char> FileReader::ReadHLSL(const char * filename);
private:
    FileReader();
};