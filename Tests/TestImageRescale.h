#pragma once

#include "..\TestSuite\Test.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"

class TestImageRescale : 
    public TestSuite::Test
{
public:
    TestImageRescale(void);
    ~TestImageRescale(void);
    void run(void);
    void testRescaleImage(void);

    typedef double InPixelType;
    typedef unsigned char OutPixelType;
    const static int Dimension = 2;
    typedef itk::Image<InPixelType, Dimension> InImageType;
    typedef itk::Image<OutPixelType, Dimension> OutImageType;
    typedef itk::ImageFileReader<InImageType> ReaderType;
    typedef itk::ImageFileWriter<OutImageType> WriterType;
    typedef itk::RescaleIntensityImageFilter<InImageType, OutImageType> RescaleType;
};
