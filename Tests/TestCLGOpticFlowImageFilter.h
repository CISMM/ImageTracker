#pragma once

#include <string>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkVector.h"

#include "../TestSuite/Test.h"
#include "../CLGOpticFlowImageFilter.h"
#include "../FilePattern.h"
#include "../FileSet.h"
#include "../FileSetImageReader.h"
#include "../Logger.h"

class TestCLGOpticFlowImageFilter :
    public TestSuite::Test
{
public:
    TestCLGOpticFlowImageFilter(){}
    virtual ~TestCLGOpticFlowImageFilter(){}

    void run()
    {
        this->testOpticFlow();
    }

    void testOpticFlow()
    {
        // Types
        Logger::logDebug("Defining types...");
        typedef itk::Image<float, 2> InputImageType;
        typedef itk::Vector<float, 2> VectorType;
        typedef itk::Image<VectorType, 2> VectorImageType;
        typedef itk::ImageFileReader<InputImageType> ReaderType;
        typedef itk::ImageFileWriter<VectorImageType> WriterType;
        typedef CLGOpticFlowImageFilter<InputImageType, InputImageType> FlowFilterType;

        // Files
        Logger::logDebug("Choosing files...");
        // std::string dir = "C:\\UNC_CS\\Research\\Nano\\data\\Fibrin\\Mucus\\";
        
        std::string dir = "C:\\UNC_CS\\Research\\Nano\\data\\test\\Quenot\\";
        FileSet* filesIn = new FileSet(new FilePattern(dir, "Ice", ".png", 1, 2, 2));
        FileSet* filesOut = new FileSet(filesIn, "CLG-", "mha");
        FileSetImageReader* reader = new FileSetImageReader(filesIn);
        reader->FirstImage();
        FileSet::FileIterator outIt = filesOut->GetFileNames()->begin();

        // Objects
        Logger::logDebug("Creating pipeline objects...");
        FlowFilterType::Pointer filter = FlowFilterType::New();
        WriterType::Pointer writer = WriterType::New();

        std::string img1, img2, out;
        while (reader->HasNext() && outIt != filesOut->GetFileNames()->end())
        {
            img1 = reader->CurrentFileName();

            // Pipeline
            Logger::logDebug("Connecting pipeline...");
            filter->SetInput1((InputImageType::ConstPointer) reader->CurrentImage());
            filter->SetInput2((InputImageType::ConstPointer) reader->NextImage());
            writer->SetInput(filter->GetOutput());
            
            img2 = reader->CurrentFileName();
            out = *outIt;
            *(outIt)++;
            writer->SetFileName(out.c_str());

            // Execute
            Logger::logDebug(img1 + " & " + img2 + " => " + out);
            Logger::logDebug("Executing...");
            writer->Update();
            Logger::logDebug("Done.");
        }
    }
};