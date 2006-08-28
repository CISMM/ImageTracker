#include <string>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkRegionOfInterestImageFilter.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "ImageUtils.h"
#include "Logger.h"

/**
 * Extracts a video that pans across a still image. Given an image,
 * geometry, starting location and ending location, we create a series
 * of images that pans from one region of an image to another.
 */
int main(int argc, char** argv)
{
    if (argc < 11)
    {
        Logger::error << "Usage: " << std::endl;
        Logger::error << "\tdir fileIn geomX geomY startX startY endX endY steps formatOut" << std::endl;
        exit(1);
    }
    
    // Typedefs
    typedef itk::Image< unsigned short, 2 > ImageType;
    typedef itk::ImageFileReader< ImageType > ReaderType;
    typedef ImageType::RegionType RegionType;
    typedef RegionType::SizeType SizeType;
    typedef RegionType::IndexType IndexType;
    typedef itk::RegionOfInterestImageFilter< ImageType, ImageType > ROIType;
    
    // Parse input
    std::string dir(argv[1]);
    std::string fileIn(argv[2]);
    SizeType size;
    size[0] = atoi(argv[3]);
    size[1] = atoi(argv[4]);
    IndexType start;
    start[0] = atoi(argv[5]);
    start[1] = atoi(argv[6]);
    IndexType end;
    end[0] = atoi(argv[7]);
    end[1] = atoi(argv[8]);
    int steps = atoi(argv[9]);
    std::string formatOut(argv[10]);
    FileSet filesOut(FilePattern(dir, formatOut, 0, steps-1));
    
    Logger::debug << "Geometry:\t" << size[0] << ", " << size[1] << std::endl;
    Logger::debug << "Start:\t" << start[0] << ", " << start[1] << std::endl;
    Logger::debug << "Finish:\t" << end[0] << ", " << end[1] << std::endl;
    
    // Determine distance between frames
    double stepXY[2];
    stepXY[0] = (double) (end[0] - start[0]) / (steps-1);
    stepXY[1] = (double) (end[1] - start[1]) / (steps-1);
    Logger::debug << "Spacing:\t" << stepXY[0] << ", " << stepXY[1] << std::endl;

    // Read static image
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(fileIn.c_str());
    reader->Update();

    // Set up ROI filter
    ROIType::Pointer roi = ROIType::New();
    roi->SetInput(reader->GetOutput());
    
    // Set up regions of interest
    RegionType region;
    region.SetSize(size);

    IndexType index;
    double posXY[2];
    posXY[0] = start[0];
    posXY[1] = start[1];

    Logger::debug << "Generating panning sequence." << std::endl;
    for (unsigned int i = 0; i < steps; i++)
    {
        index[0] = static_cast<IndexType::IndexValueType>(posXY[0]);
        index[1] = static_cast<IndexType::IndexValueType>(posXY[1]);
        region.SetIndex(index);
        roi->SetRegionOfInterest(region);
        roi->Update();
        WriteImage(roi->GetOutput(), filesOut[i]);
        posXY[0] += stepXY[0];
        posXY[1] += stepXY[1];
    }
}
