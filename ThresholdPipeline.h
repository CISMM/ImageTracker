#pragma once

#include "itkImage.h"
#include "itkThresholdImageFilter.h"

#include "CommonTypes.h"
#include "FileSet.h"

class ThresholdPipeline
{
public:
    ThresholdPipeline(void);
    ~ThresholdPipeline(void);

	typedef CommonTypes::ImageType ImageType;
    typedef ImageType::PixelType PixelType;
    typedef itk::ThresholdImageFilter<ImageType> ThresholdType;

    void Update(void);
    void ThresholdAbove(PixelType& above);
    void ThresholdBelow(PixelType& below);
    void ThresholdOutside(PixelType& lower, PixelType& upper);
    void SetInputFiles(FileSet* files);
    void SetOutputFiles(FileSet* files);

private:
    ThresholdType::Pointer thresholdFilter;
    FileSet* inFiles;
    FileSet* outFiles;
};
