#include <cmath>
#include <string>

#include "itkImage.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkMultiplyImageFilter.h"
#include "itkRecursiveGaussianImageFilter.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"
#include "Logger.h"

/**
 * Modulates a video with a set of strips of semi-transparent occlusions.  The user
 * supplies the number of strips to create; the strips are vertical blocks, evenly
 * divided over the width of the image plane.  The attenuation map is a step function
 * with both positive and negative edges.
 */
int main(int argc, char** argv)
{
    // Check parameters
    if (argc < 8)
    {
        Logger::error << "Usage:\n\t" << argv[0] << " dir formatIn start end formatOut modFile strips" << std::endl;
        exit(1);
    }
    
    Logger::verbose << "Parsing arguments" << std::endl;
    std::string dir = argv[1];
    std::string formatIn = argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string formatOut = argv[5];
    std::string modFile = argv[6];
    int strips = atoi(argv[7]);
    
    // Define types
    const unsigned int Dimension = 2;
    typedef itk::Image< unsigned short, Dimension > InputImageType;
    typedef itk::Image< float, Dimension > ModulateImageType;
    typedef itk::MultiplyImageFilter< InputImageType, ModulateImageType, InputImageType > MultiplyType;
    typedef itk::RecursiveGaussianImageFilter< ModulateImageType, ModulateImageType > SmoothType;
    
    typedef itk::ImageRegionIteratorWithIndex< ModulateImageType > IteratorType;
    typedef ModulateImageType::RegionType RegionType;
    typedef ModulateImageType::IndexType IndexType;
    typedef ModulateImageType::SizeType SizeType;
    
    Logger::verbose << "Setting up input images" << std::endl;
    FileSet filesIn(FilePattern(dir, formatIn, start, end));
    FileSet filesOut(FilePattern(dir, formatOut, start, end));
    ImageSetReader<InputImageType> video(filesIn);
    
    Logger::verbose << "Setting up attenuation image" << std::endl;
    InputImageType::SizeType size = video[0]->GetLargestPossibleRegion().GetSize();
    IndexType index;
    index.Fill(0);
    RegionType region;
    region.SetIndex(index);
    region.SetSize(size);
    ModulateImageType::Pointer modImg = ModulateImageType::New();
    modImg->SetRegions(region);
    modImg->Allocate();
    
    Logger::verbose << "Computing attenuation image" << std::endl;
    // int stripWidth = (int) size[0] / (2 * strips - 1);
    int stripWidth = (int) size[0] / (strips);
    // 1 2 3 4 5 6 7
    double mod = 1.0 / strips;
    IteratorType it(modImg, region);
    
    for (it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
        index = it.GetIndex();
        int strip = 1 + (int) index[0] / stripWidth;
        // strip = std::min(strip, 2*strips-1);
        strip = std::min(strip, strips);
        it.Set(mod * (1 + std::abs(strips - strip)));
    }
    
    Logger::verbose << "Smoothing attenuation map" << std::endl;
    SmoothType::Pointer smoothX = SmoothType::New();
    smoothX->SetDirection(0);
    smoothX->SetZeroOrder();
    smoothX->SetSigma(1.0);
    SmoothType::Pointer smoothY = SmoothType::New();
    smoothY->SetDirection(1);
    smoothY->SetZeroOrder();
    smoothY->SetSigma(1.0);
    
    smoothX->SetInput(modImg);
    smoothY->SetInput(smoothX->GetOutput());
    
    WriteImage<ModulateImageType>(smoothY->GetOutput(), modFile);
    
    Logger::verbose << "Modulating input images" << std::endl;
    MultiplyType::Pointer mult = MultiplyType::New();
    mult->SetInput2(smoothY->GetOutput());
    for (int i = 0; i < video.size(); i++)
    {
        mult->SetInput1(video[i]);
        WriteImage(mult->GetOutput(), filesOut[i]);
    }
}