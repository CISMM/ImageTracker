
#include <string>

#include "itkImage.h"
#include "itkVector.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"
#include "Logger.h"
#include "NaryMeanVectorImageFilter.h"

int main(int argc, char** argv)
{
    if (argc < 6)
    {
        Logger::error << "Usage: " << std::endl;
        Logger::error << "\t" << argv[0] << " dir format start stop meanImg" << std::endl;
        return 1;
    }

    // Typedefs
    typedef itk::Vector< float, 2 > PixelType;
    typedef itk::Image< PixelType > ImageType;
    typedef ImageSetReader< ImageType > ReaderType;
    typedef NaryMeanVectorImageFilter< ImageType, ImageType > MeanType;

    // Parse arguments
    std::string dir = argv[1];
    std::string format = argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string meanImg = argv[5];

    FileSet files(FilePattern(dir, format, start, end));
    ReaderType images(files);
    MeanType::Pointer mean = MeanType::New();

    for (unsigned int i = 0; i < images.size(); i++)
    {
        mean->AddInput(images[i]);
    }

    WriteImage< ImageType >(mean->GetOutput(), meanImg);
    
    images.LogStatistics();
}
