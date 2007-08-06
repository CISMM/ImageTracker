
#include <string>

#include "itkImage.h"
#include "itkImageRegionConstIterator.h"
#include "itkRegionOfInterestImageFilter.h"
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
        Logger::error << "\t" << argv[0] << " dir format start stop meanImg [roiRadius]" << std::endl;
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
    typedef itk::RegionOfInterestImageFilter< ImageType, ImageType > ROIType;

    // Parse arguments
    std::string dir = argv[1];
    std::string format = argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string meanImg = argv[5];
    int radius = argc > 6 ? atoi(argv[6]) : 0;

    FileSet files(FilePattern(dir, format, start, end));
    ReaderType images(files);
    ROIType::Pointer roi = ROIType::New();
    MeanType::Pointer mean = MeanType::New();

    images[0]->Update();
    roi->SetRegionOfInterest(PadRegionByRadius(images[0]->GetLargestPossibleRegion(), radius));
    
    for (unsigned int i = 0; i < images.size(); i++)
    {
        roi->SetInput(images[i]);
        mean->AddInput(roi->GetOutput());
    }
    
    WriteImage< ImageType >(mean->GetOutput(), meanImg);
    PrintImageInfo(mean->GetOutput(), "Mean image");
}
