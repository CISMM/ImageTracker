#include "ThresholdPipeline.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

ThresholdPipeline::ThresholdPipeline(void)
{
    this->thresholdFilter = ThresholdType::New();
}

ThresholdPipeline::~ThresholdPipeline(void)
{
}

void ThresholdPipeline::Update()
{
    typedef itk::ImageFileReader<ImageType> ReaderType;
    typedef itk::ImageFileWriter<ImageType> WriterType;

    ReaderType::Pointer reader = ReaderType::New();
    WriterType::Pointer writer = WriterType::New();

    FileSet::FileIterator inIt = this->inFiles->GetFileNames()->begin();
    FileSet::FileIterator outIt = this->outFiles->GetFileNames()->begin();

    for ( ; inIt != this->inFiles->GetFileNames()->end() &&
        outIt != this->outFiles->GetFileNames()->end(); *inIt++, *outIt++)
    {
        std::string inFile = this->inFiles->GetDirectory() + *inIt;
        std::string outFile = this->outFiles->GetDirectory() + *outIt;

        //writer->ResetPipeline();
        this->thresholdFilter->SetInput(reader->GetOutput());
        writer->SetInput(this->thresholdFilter->GetOutput());

        reader->SetFileName(inFile.c_str());
        writer->SetFileName(outFile.c_str());
        writer->Update();
    }
}

void ThresholdPipeline::ThresholdAbove(PixelType& above)
{
    this->thresholdFilter->ThresholdAbove(above);
}

void ThresholdPipeline::ThresholdBelow(PixelType& below)
{
    this->thresholdFilter->ThresholdBelow(below);
}

void ThresholdPipeline::ThresholdOutside(PixelType& lower, PixelType& upper)
{
    this->thresholdFilter->ThresholdOutside(lower, upper);
}

void ThresholdPipeline::SetInputFiles(FileSet* files)
{
    this->inFiles = files;
}

void ThresholdPipeline::SetOutputFiles(FileSet* files)
{
    this->outFiles = files;
}