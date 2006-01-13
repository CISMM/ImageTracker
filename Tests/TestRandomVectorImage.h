#pragma once

#include <string>
#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkRandomImageSource.h"
#include "itkVector.h"

#include "../TestSuite/Test.h"
#include "../Logger.h"

class TestRandomVectorImage : public TestSuite::Test
{
public:
    TestRandomVectorImage(){}
    ~TestRandomVectorImage(){}

    void run()
    {
        this->testCreateImage();
    }

    /**
     * Create a vector field that has constant flow randomized a little bit.
     */
    void testCreateImage()
    {
        // Define types
        typedef itk::Image<float, 2> RandomImageType;
        typedef itk::Vector<float, 2> VectorType;
        typedef itk::Image<VectorType, 2> VectorImageType;
        typedef itk::RandomImageSource<RandomImageType> RandomType;
        typedef itk::ImageFileWriter<VectorImageType> WriterType;
        typedef itk::ImageRegionConstIterator<RandomImageType> RandomIteratorType;
        typedef itk::ImageRegionIterator<VectorImageType> VectorIteratorType;
        
        // Set up randomizers
        RandomType::Pointer randX = RandomType::New();
        RandomType::Pointer randY = RandomType::New();
        randX->SetMin(-3.0); randY->SetMin(-3.0);
        randX->SetMax( 3.0); randY->SetMax( 3.0);

        Logger::logDebug("Creating random images...");
        unsigned long rSize[2] = {256, 256};
        float rOrigin[2] = {0.0f, 0.0f};
        float rSpacing[2] = {1.0f, 1.0f};
        randX->SetOrigin(rOrigin);      randY->SetOrigin(rOrigin);
        randX->SetSize(rSize);          randY->SetSize(rSize);
        randX->SetSpacing(rSpacing);    randY->SetSpacing(rSpacing);
        randX->Update();                randY->Update();

        // Set up Vector image
        VectorImageType::IndexType origin;
        origin[0] = 0; origin[1] = 0;
        VectorImageType::SizeType size;
        size[0] = 256; size[1] = 256;
        VectorImageType::SpacingType spacing;
        spacing[0] = 1; spacing[1] = 1;

        Logger::logDebug("Creating vector image...");
        VectorImageType::Pointer vecImg = VectorImageType::New();
        VectorImageType::RegionType region;
        region.SetSize(size);
        region.SetIndex(origin);
        vecImg->SetRegions(region);
        vecImg->SetSpacing(spacing);
        vecImg->Allocate();

        VectorType vec;
        VectorType constOut, constIn;
        constOut[0] = 10.0;        constIn[0] = 0.0;
        constOut[1] = 0.0;         constIn[1] = -10.0;

        // Create image iterators
        RandomIteratorType outX(randX->GetOutput(), region);
        RandomIteratorType outY(randY->GetOutput(), region);
        VectorIteratorType outVec(vecImg, region);
        
        // Create vector image (whole image)
        Logger::logDebug("Computing image data...");
        for (outX.GoToBegin(), outY.GoToBegin(), outVec.GoToBegin();
            !outX.IsAtEnd() && !outY.IsAtEnd() && !outVec.IsAtEnd();
            ++outX, ++outY, ++outVec)
        {
            vec[0] = constOut[0] + outX.Get();
            vec[1] = constOut[1] + outY.Get();
            outVec.Set(vec);
        }

        // Create a different region in center of image.
        VectorImageType::IndexType inOrigin;
        inOrigin[0] = 64;   inOrigin[1] = 64;
        VectorImageType::SizeType inSize;
        inSize[0] = 128;    inSize[1] = 128;
        VectorImageType::RegionType inRegion;
        inRegion.SetSize(inSize);
        inRegion.SetIndex(inOrigin);

        RandomIteratorType inX(randX->GetOutput(), inRegion);
        RandomIteratorType inY(randY->GetOutput(), inRegion);
        VectorIteratorType inVec(vecImg, inRegion);

        Logger::logDebug("Creating an inner region...");
        for (inX.GoToBegin(), inY.GoToBegin(), inVec.GoToBegin();
            !inX.IsAtEnd() && !inY.IsAtEnd() && !inVec.IsAtEnd();
            ++inX, ++inY, ++inVec)
        {
            vec[0] = constIn[0] + inX.Get();
            vec[1] = constIn[1] + inY.Get();
            inVec.Set(vec);
        }

        // Save image
        std::string dir = "C:\\UNC_CS\\Research\\Nano\\data\\test\\";
        std::string file = "FlowRegions.mha";
        std::string filename = dir + file;

        std::cout << "Writing vector image" << std::endl;
        WriterType::Pointer writer = WriterType::New();
        writer->SetFileName(filename.c_str());
        writer->SetInput(vecImg);
        writer->Update();

        Logger::logDebug("Done.");
    }
};