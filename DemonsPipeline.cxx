#include "DemonsPipeline.h"

#include "itkDemonsRegistrationFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkHistogramMatchingImageFilter.h"
#include "itkVector.h"

#include "CommonTypes.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"

DemonsPipeline::DemonsPipeline(void)
{
}

DemonsPipeline::~DemonsPipeline(void)
{
}

void DemonsPipeline::Update()
{
    if (this->inFiles.size() < 2 ||
        this->outFiles.size() == 0)
    {
        std::cerr << "DemonsPipeline::Update() Warning: input or output files improperly configured." << std::endl;
        return;
    }

    // Define types used
    typedef CommonTypes::InputImageType InputImageType;
    typedef CommonTypes::InternalImageType InternalImageType;
    typedef ImageSetReader<InputImageType, InternalImageType> ReaderType;
    typedef itk::HistogramMatchingImageFilter<ImageType, ImageType> MatchingFilterType;
    typedef itk::Vector<float, 2> VectorType;
    typedef itk::Image<VectorType, 2> VectorImageType;
    typedef itk::DemonsRegistrationFilter<ImageType, ImageType, VectorImageType> RegistrationType;
    
    // Instantiate processing objects
    MatchingFilterType::Pointer matcher = MatchingFilterType::New();
    RegistrationType::Pointer demons = RegistrationType::New();

    ReaderType reader(this->inFiles);
    std::cout << "Reading => " << this->inFiles[0] << std::endl;
    ImageType::Pointer movingImage = reader[0];
    ImageType::Pointer fixedImage;

    // Process each image
    for (unsigned int i = 0; i < reader.size()-1; i++)
    {
        fixedImage = reader[i+1];
        std::cout << "Reading => " << this->inFiles[i+1] << std::endl;
        
        // Histogram match (required for Demons)
        matcher->SetInput(movingImage);
        matcher->SetReferenceImage(fixedImage);
        matcher->SetNumberOfHistogramLevels(1024);
        matcher->SetNumberOfMatchPoints(7);
        matcher->ThresholdAtMeanIntensityOn();

        // Set up Demons
        demons->SetFixedImage(fixedImage);
        demons->SetMovingImage(matcher->GetOutput());
        demons->SetNumberOfIterations(150);
        demons->SetStandardDeviations(1.0);

        demons->Update();

        // Write vector field output
        std::cout << "Writing => " << this->outFiles[i] << std::endl;
        WriteImage(demons->GetOutput(), this->outFiles[i]);

        movingImage = fixedImage;
    }
}

void DemonsPipeline::SetInputFiles(const FileSet& files)
{
    this->inFiles = files;
}

void DemonsPipeline::SetOutputFiles(const FileSet& files)
{
    this->outFiles = files;
}
