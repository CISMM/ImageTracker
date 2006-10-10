
#include <string>

#include "itkCenteredRigid2DTransform.h"
#include "itkImage.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkResampleImageFilter.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"
#include "Logger.h"

/** 
 * Creates a video from a single image comprised of a region of interest about which the
 * original image rotates.  If a carefully chosed image region is chosen, no pixels outside
 * the original image's field of view will be visible.
 */
int main(int argc, char** argv)
{
    // Check input arguments
    if (argc < 11)
    {
        Logger::error << "Usage:\n\t" << argv[0] << " dir fileIn sizeX sizeY centerX centerY startAngle endAngle steps formatOut" << std::endl;
        Logger::error << "\tAngles are in degrees" << std::endl;
        exit(1); 
    }
    
    // typedefs
    typedef itk::Image< unsigned short, 2 > ImageType;
    typedef ImageType::RegionType RegionType;
    typedef RegionType::SizeType SizeType;
    typedef RegionType::IndexType IndexType;
    
    // Note: precision has to be double.  ResampleImageFilter doesn't work with float.
    typedef itk::CenteredRigid2DTransform< double > TransformType;
    typedef itk::ResampleImageFilter< ImageType, ImageType, double > ResampleType;
    typedef itk::RegionOfInterestImageFilter< ImageType, ImageType > ROIType;
    typedef TransformType::InputPointType PointType;
    
    const double PI = 3.14159265;
    
    Logger::verbose << "Parsing inputs" << std::endl;
    std::string dir = argv[1];
    std::string fileIn = argv[2];
    SizeType size;
    size[0] = atoi(argv[3]);
    size[1] = atoi(argv[4]);
    PointType center;
    center[0] = atoi(argv[5]);
    center[1] = atoi(argv[6]);
    IndexType index;
    index[0] = center[0] - (size[0]/2);
    index[1] = center[1] - (size[1]/2);
    double radianStart = atof(argv[7]) * PI / 180;
    double radianEnd = atof(argv[8]) * PI / 180;
    int steps = atoi(argv[9]);
    std::string formatOut = argv[10];
    double radStep = (radianEnd - radianStart) / (steps - 1);
    
    RegionType region;
    region.SetSize(size);
    region.SetIndex(index);
    Logger::verbose << "Output region: idx(" << index[0] << ", " << index[1] << "), size(" << size[0] << ", " << size[1] << ")" << std::endl;
    
    FileSet filesOut(FilePattern(dir, formatOut, 0, steps-1));
    
    Logger::verbose << "Creating pipeline" << std::endl;
    ImageType::Pointer image = ReadImage< ImageType >(dir + "/" + fileIn);
    TransformType::Pointer transform = TransformType::New();
    ResampleType::Pointer resample = ResampleType::New();
    ROIType::Pointer roi = ROIType::New();
    
    transform->SetIdentity();
    transform->SetCenter(center);
    resample->SetInput(image);
    resample->SetTransform(transform);
    resample->SetSize(image->GetLargestPossibleRegion().GetSize());
    resample->SetOutputOrigin(image->GetOrigin());
    resample->SetOutputSpacing(image->GetSpacing());
    resample->SetDefaultPixelValue(0);
    roi->SetRegionOfInterest(region);
    roi->SetInput(resample->GetOutput());
    
    Logger::verbose << "Generating transformed images" << std::endl;
    for (int i = 0; i < steps; i++)
    {
        double angle = radianStart + i * radStep;
        // Logger::verbose << "Angle: " << angle << " rad" << std::endl;
        transform->SetAngle(angle);
        // resample->SetTransform(transform);
        WriteImage(roi->GetOutput(), filesOut[i]);
    }
}
