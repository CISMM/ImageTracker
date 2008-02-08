#include <string>

#include "itkImage.h"
#include "itkImageSeriesReader.h"
#include "itkNumericSeriesFileNames.h"
#include "itkImageFileWriter.h"

int main(int argc, char** argv)
{
    if (argc < 5)
    {
        std::cerr << "Usage:\n\t" << argv[0] << " patternIn start end fileOut" << std::endl;
        exit(1);
    }
    
    std::string patternIn(argv[1]);
    int start = atoi(argv[2]);
    int end = atoi(argv[3]);
    std::string fileOut(argv[4]);
    
    typedef itk::Image< unsigned char, 3 > ImageType;
    typedef itk::NumericSeriesFileNames SeriesType;
    typedef itk::ImageSeriesReader< ImageType > ReaderType;
    typedef itk::ImageFileWriter< ImageType > WriterType;
    
    SeriesType::Pointer series = SeriesType::New();
    series->SetSeriesFormat(patternIn.c_str());
    series->SetStartIndex(start);
    series->SetEndIndex(end);
    
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileNames(series->GetFileNames());
    
    WriterType::Pointer writer = WriterType::New();
    writer->SetInput(reader->GetOutput());
    writer->Update();
}
