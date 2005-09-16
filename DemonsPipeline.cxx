#include "DemonsPipeline.h"

#include "itkDemonsRegistrationFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkHistogramMatchingImageFilter.h"
#include "itkVector.h"

#include "FileSetImageReader.h"

DemonsPipeline::DemonsPipeline(void)
{
}

DemonsPipeline::~DemonsPipeline(void)
{
}

void DemonsPipeline::Update()
{
    if (this->inFiles == NULL ||
        this->outFiles == NULL ||
        this->inFiles->GetFileNames()->size() < 2)
    {
        std::cerr << "DemonsPipeline::Update() Warning: input or output files improperly configured." << std::endl;
        return;
    }

    // Define types used
    typedef itk::HistogramMatchingImageFilter<ImageType, ImageType> MatchingFilterType;
    typedef itk::Vector<float, 2> VectorType;
    typedef itk::Image<VectorType, 2> VectorImageType;
    typedef itk::DemonsRegistrationFilter<ImageType, ImageType, VectorImageType> RegistrationType;
    typedef itk::ImageFileWriter<VectorImageType> WriterType;
    
    // Instantiate processing objects
    MatchingFilterType::Pointer matcher = MatchingFilterType::New();
    RegistrationType::Pointer demons = RegistrationType::New();

    FileSetImageReader* pReader = new FileSetImageReader(this->inFiles);
    std::cout << "Reading => " << pReader->CurrentFileName() << std::endl;
    ImageType::Pointer movingImage = pReader->CurrentImage();
    ImageType::Pointer fixedImage;

    WriterType::Pointer writer = WriterType::New();
    FileSet::FileIterator outIt = this->outFiles->GetFileNames()->begin();

    // Process each image
    while (pReader->HasNext())
    {
        fixedImage = pReader->NextImage();
        std::cout << "Reading => " << pReader->CurrentFileName() << std::endl;
        
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
        std::string outFile = *outIt;
        std::cout << "Writing => " << outFile << std::endl;
        writer->SetFileName(outFile.c_str());
        *outIt++;
        writer->SetInput(demons->GetOutput());
        writer->Update();

        movingImage = fixedImage;
    }

    delete pReader;
}

void DemonsPipeline::SetInputFiles(FileSet* files)
{
    this->inFiles = files;
}

void DemonsPipeline::SetOutputFiles(FileSet* files)
{
    this->outFiles = files;
}