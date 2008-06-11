#include <string>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

/**
 * Converts sequences of scalar-pixeled images using ITK.
 */
int main(int argc, char** argv)
{
    if (argc < 5)
    {
        std::cerr << "Usage:\n\t" << argv[0] << " patternIn start end patternOut" << std::endl;
        exit(1);
    }
    
    std::string patternIn(argv[1]);
    int start = atoi(argv[2]);
    int end = atoi(argv[3]);
    std::string patternOut(argv[4]);
    
    typedef itk::Image< unsigned char, 2 > ImageType;
    typedef itk::ImageFileReader<ImageType> ReaderType;
    typedef itk::ImageFileWriter< ImageType > WriterType;
    
    ReaderType::Pointer reader = ReaderType::New();
    WriterType::Pointer writer = WriterType::New();
    
    writer->SetInput(reader->GetOutput());
    
    char fileIn[128];
    char fileOut[128];
    
    for (int i = start; i <= end; ++i)
    {
        sprintf(fileIn, patternIn.c_str(), i);
        sprintf(fileOut, patternOut.c_str(), i);
        reader->SetFileName(fileIn);
        writer->SetFileName(fileOut);
        writer->Update();
    }
    
}
