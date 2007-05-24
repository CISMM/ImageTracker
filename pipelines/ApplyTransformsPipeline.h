#pragma once

#include <string>

#include "itkImage.h"
#include "itkObject.h"

#include "CommonTypes.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ItkPipeline.h"

/**
 * \class ApplyTransformsPipeline
 * \brief Applies a set of rigid transforms to a set of images.
 * Given a set of files and a file containing a list of transforms, this
 * pipeline applies the set of transforms to the set of images.  The transforms
 * are composed, so each applied transform is a composition of all previous
 * transforms.
 */
class ApplyTransformsPipeline :
    public ItkPipeline
{
public:
    // Standard itk typedefs
    typedef ApplyTransformsPipeline Self;
    typedef ItkPipeline Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    itkNewMacro(Self);
    
    itkGetMacro(TransformFile, std::string);
    itkSetMacro(TransformFile, std::string);
    
    //void SetInput(ImageSetReaderBase* input) { this->input = input; }
    //const FileSet& GetOutputFiles() { return this->outputFiles; }
    //void SetOutputFiles(const FileSet& files) { this->outputFiles = files; }

    virtual void Update();
    
    typedef CommonTypes::InputImageType WriteImageType;
    typedef CommonTypes::InternalImageType InternalImageType;
    
protected:
    ApplyTransformsPipeline() {}
    virtual ~ApplyTransformsPipeline() {}
    
private:
    // not implemented
    ApplyTransformsPipeline(const Self& other);
    void operator=(const Self& other);
    
    //ImageSetReaderBase* input;
    //FileSet outputFiles;
    std::string m_TransformFile;
};
