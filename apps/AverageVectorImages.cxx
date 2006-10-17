
#include <string>

#include "itkImage.h"
#include "itkImageRegionConstIterator.h"
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
    const unsigned int Dimension = 2;
    typedef float ElementType;
    typedef itk::Vector< ElementType, Dimension > PixelType;
    typedef itk::Vector< itk::NumericTraits< ElementType >::AccumulateType, Dimension > AccumType;
    typedef itk::Image< PixelType, 2 > ImageType;
    typedef ImageSetReader< ImageType > ReaderType;
    typedef NaryMeanVectorImageFilter< ImageType, ImageType > MeanType;
    typedef itk::ImageRegionConstIterator< ImageType > IteratorType;

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
    
    // Compute the mean vector of the mean image
    IteratorType iter(mean->GetOutput(), mean->GetOutput()->GetLargestPossibleRegion());
    AccumType sum;
    sum.Fill(0.0);
    for (iter.GoToBegin(); !iter.IsAtEnd(); ++iter)
    {
        sum += iter.Get();
    }
    
    ImageType::SizeType size = mean->GetOutput()->GetLargestPossibleRegion().GetSize();
    unsigned int pixels = 1;
    Logger::verbose << "Mean image size: [";
    for (int d = 0; d < Dimension; d++)
    {
        pixels *= size[d];
        Logger::verbose << size[d] << (d == Dimension-1 ? "]" : ", ");
    }
    Logger::verbose << ", pixel count: " << pixels << std::endl;
    
    AccumType meanmean = sum / pixels;
    Logger::verbose << "Mean vector of mean image: [";
    for (int d = 0; d < Dimension; d++)
    {
        Logger::verbose << meanmean[d] << (d == Dimension-1 ? "]\n" : ", ");
    }
    
    images.LogStatistics();
}
