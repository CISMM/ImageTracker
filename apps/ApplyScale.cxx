#include <string>

#include "itkImage.h"
#include "itkUnaryFunctorImageFilter.h"
#include "itkVector.h"
#include "itkVectorRescaleIntensityImageFilter.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"

int main(int argc, char** argv)
{
    std::string function(__FILE__);
    
    // Check command-line arguments
    if (argc < 7)
    {
        Logger::warning << "Usage:\n\t" << argv[0] << " dir formatIn start end formatOut spatialScale [valueScale]" << std::endl;
        Logger::warning << "\tspatialScale - e.g. microns per pixel spacing" << std::endl;
        Logger::warning << "\tvalueScale   - e.g. microns per unit intensity (optional rescaling)" << std::endl;
        exit(1);
    }
    
    Logger::debug << function << ": parsing command line arguments" << std::endl;
    std::string dir(argv[1]);
    std::string formatIn(argv[2]);
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string formatOut(argv[5]);
    float spatialScale = atof(argv[6]);
    float valueScale = argc > 7 ? atof(argv[7]) : 1.0;
    
    // Typedefs
    typedef itk::Vector< float, 2 > PixelType;
    typedef itk::Image< PixelType, 2 > ImageType;
    typedef itk::UnaryFunctorImageFilter< ImageType, ImageType, itk::Functor::VectorMagnitudeLinearTransform< PixelType, PixelType > > ScaleType;
    
    Logger::debug << function << ": setting up image IO" << std::endl;
    FileSet filesIn(FilePattern(dir, formatIn, start, end));
    FileSet filesOut(FilePattern(dir, formatOut, start, end));
    ImageSetReader< ImageType > video(filesIn);
    
    Logger::debug << function << ": setting up rescaling pipeline" << std::endl;
    ScaleType::Pointer scale = ScaleType::New();
    scale->GetFunctor().SetFactor(valueScale);
    
    ImageType::SpacingType spacing;
    spacing.Fill(spatialScale);
    
    Logger::debug << function << ": executing rescaling" << std::endl;
    for (int i = 0; i < video.size(); i++)
    {
        video[i]->SetSpacing(spacing);
        scale->SetInput(video[i]);
        WriteImage(scale->GetOutput(), filesOut[i]);
    }
    
    Logger::debug << function << ": done." << std::endl;
}
