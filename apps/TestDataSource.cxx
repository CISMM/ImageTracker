#include <string>

#include "itkDataObject.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkCastImageFilter.h"
#include "itkRecursiveGaussianImageFilter.h"

#include "DataSource.h"
#include "FilePattern.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"
#include "Logger.h"

/**
 * Smoothes a set of images with a Gaussian filter, using a DataSource.
 */
int main(int argc, char** argv)
{
    // Check input count
    if (argc < 7)
    {
        Logger::error << "Usage:\n\t" << argv[0] << " dir formatIn start stop formatOut sigma" << std::endl;
        exit(1);
    }
    
    typedef itk::Image< unsigned char, 2 > InputImageType;
    typedef itk::Image< float, 2 > InternalImageType;
    typedef itk::RecursiveGaussianImageFilter< InternalImageType, InternalImageType > FilterType;
    
    Logger::verbose << "Parsing inputs" << std::endl;
    std::string dir         = argv[1];
    std::string formatIn    = argv[2];
    int start               = atoi(argv[3]);
    int stop                = atoi(argv[4]);
    std::string formatOut   = argv[5];
    float sigma          = atof(argv[6]);
    
    Logger::verbose << "Setting up IO components" << std::endl;
    FileSet filesIn(FilePattern(dir, formatIn, start, stop));
    FileSet filesOut(FilePattern(dir, formatOut, start, stop));
    
    Logger::verbose << "Creating pipeline components" << std::endl;
    DataSource::Pointer data = DataSource::New();
    FilterType::Pointer filter = FilterType::New();
    data->SetPixelDataType(ScalarChar);
    data->SetFiles(filesIn);
    filter->SetZeroOrder();
    filter->SetSigma(sigma);
    
    Logger::verbose << "Executing pipeline" << std::endl;
    for (int i = 0; i < filesIn.size(); i++)
    {
        filter->SetInput(dynamic_cast< InternalImageType* >(data->GetImage(i)));
        WriteImage< InternalImageType, InputImageType >(filter->GetOutput(), filesOut[i], false);
    }
    
    DataSource::Pointer empty = DataSource::New();
    Logger::verbose << "Empty data source:\n\tsize() = " << empty->size() << std::endl;
    Logger::verbose << "\tGetImage(0) = " << empty->GetImage(0) << std::endl;
}
