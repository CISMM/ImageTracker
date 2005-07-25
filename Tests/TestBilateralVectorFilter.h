#pragma once

#include <string>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkVector.h"

#include "../TestSuite/Test.h"

#include "../BilateralVectorImageFilter.h"
#include "../Gaussian2DVectorFilter.h"
#include "../Logger.h"

/**
 * \class TestBilateralVectorFilter
 * \brief Tests edge-preserving smoothing for vector images.
 * Tests the operation of a vector smoothing bilateral filter.
 * Also runs a plain Gaussian-based smoothing for comparison.
 */
class TestBilateralVectorFilter : 
    public TestSuite::Test
{
public:
    TestBilateralVectorFilter(){}
    ~TestBilateralVectorFilter(){}

    void run()
    {
        this->testReadFilterWrite();
    }

    void testReadFilterWrite()
    {
        // Define filter types
        typedef itk::Vector<float, 2> VectorType;
        typedef itk::Image<VectorType, 2> ImageType;
        typedef itk::ImageFileReader<ImageType> ReaderType;
        typedef itk::ImageFileWriter<ImageType> WriterType;
        typedef itk::BilateralVectorImageFilter<ImageType, ImageType> FilterType;
        typedef Gaussian2DVectorFilter<ImageType, ImageType> GaussFilterType;

        // Setup filter pipeline objects
        ReaderType::Pointer reader = ReaderType::New();
        FilterType::Pointer filter = FilterType::New();
        WriterType::Pointer writer = WriterType::New();
        GaussFilterType::Pointer gauss = GaussFilterType::New();
        filter->SetDomainSigma(4.0);
        filter->SetRangeSigma(0.3);
        gauss->SetVariance(16.0);
        
        // Choose filenames
        std::string dir = "C:\\UNC_CS\\Research\\Nano\\data\\test\\";
        std::string file = "FlowRegions.mha";
        std::string inFile = dir + file;
        std::string outFile = dir + "Smooth-" + file;
        std::string gaussFile = dir + "Gauss-" + file;
        
        // Run the Bilateral filter
        Logger::logDebug("Reading: " + inFile);
        reader->SetFileName(inFile.c_str());
        filter->SetInput(reader->GetOutput());
        writer->SetInput(filter->GetOutput());
        writer->SetFileName(outFile.c_str());
        Logger::logDebug("Writing: " + outFile);
        writer->Update();

        // Check out Gaussian-based smoothing.
        gauss->SetInput(reader->GetOutput());
        writer->SetInput(gauss->GetOutput());
        writer->SetFileName(gaussFile.c_str());
        Logger::logDebug("Writing: " + gaussFile);
        writer->Update();
    }
};