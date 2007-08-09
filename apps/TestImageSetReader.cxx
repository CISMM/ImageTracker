#include <string>

#include "itkImage.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "Logger.h"

int main(int argc, char** argv)
{
    std::string dir("/home/beastwoo/Nano/data/test/Quenot");
    std::string format("Ice%02d.png");
    int start = 0; 
    int end = 20;
    FileSet files(FilePattern(dir, format, start, end));
    
    typedef itk::Image< unsigned char, 2 > ImageType;
    typedef ImageSetReader< ImageType, ImageType > ReaderType;
    
    {
        Logger::debug << "********** Testing reader with radius 0, incremental access **********" << std::endl;
        ReaderType reader(files);
        for (int i = 0; i < reader.size(); i++)
            reader[i];
        for (int i = reader.size()-1; i >= 0; i--)
            reader[i];
        
        Logger::debug << "********** Testing reader with radius 0, random access **********" << std::endl;
        reader[0];
        reader[6];
        reader[10];
        reader[2];
        reader[19];
        reader[16];
    }
    
    {
        Logger::debug << "********** Testing reader with radius 1, incremental access **********" << std::endl;
        ReaderType reader(files);
        reader.SetCacheRadius(1);
        for (int i = 0; i < reader.size(); i++)
            reader[i];
    }
    
    {
        Logger::debug << "********** Testing reader with radius 5, incremental access **********" << std::endl;
        ReaderType reader(files);
        reader.SetCacheRadius(5);
        for (int i = 0; i < reader.size(); i++)
            reader[i];
        for (int i = reader.size()-1; i >= 0; i--)
            reader[i];
        
        Logger::debug << "********** Testing reader with radius 5, random access **********" << std::endl;
        reader[0];
        reader[6];
        reader[10];
        reader[2];
        reader[19];
        reader[16];
    }
    
    {
        Logger::debug << "********** Testing reader with radius 0 **********" << std::endl;
        ReaderType reader(files);
        for (int i = 0; i < reader.size(); i++)
            reader[i];
            
        reader[0];
        reader[6];
        reader[10];
        reader[2];
        reader[19];
        reader[16];
        
        Logger::debug << "********** Resetting reader radius from 0 to 5 **********" << std::endl;
        reader.SetCacheRadius(5);
        for (int i = 0; i < reader.size(); i++)
            reader[i];
            
        reader[0];
        reader[14];
//         reader[6];
//         reader[10];
//         reader[2];
//         reader[19];
//         reader[16];
        
        Logger::debug << "********** Requesting an invalid image index **********" << std::endl;
        reader[30];
    }
}
