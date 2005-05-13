#pragma once
#include "../TestSuite/Test.h"
#include <string>
#include <vector>

#include "itkCovariantVector.h"
#include "itkGradientRecursiveGaussianImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"
#include "itkVector.h"

#include "../Logger.h"

class TestVectorWrite : public TestSuite::Test
{
public:
    TestVectorWrite(){}
    ~TestVectorWrite(){}

    void run()
    {
        this->testWriteVector();
    }

    void testWriteVector()
    {
        Logger::logDebug("Setting up typedefs.");
        typedef itk::Image<unsigned char, 2> ImageType;
        typedef itk::ImageFileReader<ImageType> ReaderType;

        typedef itk::Vector<float, 2> VectorType;
        typedef itk::Image<VectorType, 2> VectorImageType;
        typedef itk::ImageFileWriter<VectorImageType> WriterType;
        typedef itk::ImageRegionIterator<VectorImageType> IteratorType;

        typedef itk::GradientRecursiveGaussianImageFilter<ImageType, VectorImageType> FilterType;

        Logger::logDebug("Setting up filenames.");
        std::string dir = "C:\\UNC_CS\\Research\\Nano\\data\\Fibrin\\lolo-2004-05-14-2narea\\";
        std::string name = "mani420";
        std::string inImg = dir + name + ".png";
        std::string outImg = dir + "Grad-" + name + ".mha";

        //Logger::logDebug("Setting up pipeline.");
        //ReaderType::Pointer reader = ReaderType::New();
        //FilterType::Pointer filter = FilterType::New();
        WriterType::Pointer writer = WriterType::New();
        //filter->SetInput(reader->GetOutput());
        //writer->SetInput(filter->GetOutput());
        //
        //Logger::logDebug("Running pipeline...");
        //reader->SetFileName(inImg.c_str());
        //writer->SetFileName(outImg.c_str());
        //writer->Update();
        //Logger::logDebug("Done.");


        Logger::logDebug("Creating vector image.");
        // VectorImageType::Pointer vecImg = filter->GetOutput();
        VectorImageType::Pointer vecImg = VectorImageType::New();
        VectorImageType::RegionType region;
        VectorImageType::SizeType size;
        size[0] = 256; size[1] = 256;
        VectorImageType::IndexType index;
        index[0] = 0; index[1] = 0;
        region.SetSize(size);
        region.SetIndex(index);
        vecImg->SetRegions(region);

        Logger::logDebug("Allocating image.");
        vecImg->Allocate();

        //float[2] origin;
        //origin[0] = 0; origin[1] = 0;
        //float[2] spacing;
        //spacing[0] = 1; spacing[1] = 1;
        //vecImg->SetOrigin(origin);
        //vecImg->SetSpacing(spacing);

        Logger::logDebug("Filling vector image pixels.");
        IteratorType it(vecImg, vecImg->GetLargestPossibleRegion());
        for (it.GoToBegin(); !it.IsAtEnd(); ++it)
        {
            VectorImageType::PixelType pixel;
            pixel[0] = 5;
            pixel[1] = 5;
            it.Set(pixel);
        }

        Logger::logDebug("Writing image to file.");
        writer->SetFileName(outImg.c_str());
        writer->SetInput(vecImg);
        writer->Update();
    }
};