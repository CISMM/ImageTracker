#pragma once

#include "itkImage.h"
#include "itkVector.h"

class ImageStatistics
{
public:
    typedef float ComponentType;
    typedef itk::Vector<ComponentType, 2> VectorType;
    typedef itk::Image<VectorType, 2> ImageType;

    ImageStatistics(void);
    ~ImageStatistics(void);

    void SetImage(ImageType::Pointer image);
    int GetCount(void);
    double GetDeviation(void);
    double GetMaximum(void);
    double GetMean(void);
    double GetMinimum(void);
    
private:
    void DoCompute(void);
    ImageType::Pointer image;
    int count;
    double mean, deviation, minimum, maximum;
};
