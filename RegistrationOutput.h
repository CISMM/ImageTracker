#pragma once

#include <string>

#include "itkCastImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkResampleImageFilter.h"

#include "CommonTypes.h"
#include "FileSet.h"
#include "TransformGroup.h"

class RegistrationOutput
{
public:
    RegistrationOutput(FileSet* source = NULL, TransformGroup::TransformVector* transforms = NULL, FileSet* destination = NULL, int composeMode = RegistrationOutput::COMPOSE_NONE );
    ~RegistrationOutput(void);

    typedef unsigned char OutputPixelType;
    typedef itk::Image<OutputPixelType, 2> OutputImageType;
    typedef CommonTypes::ImageType InputImageType;
   
    typedef itk::ImageFileReader<InputImageType> ReaderType;
    typedef itk::ResampleImageFilter<InputImageType, InputImageType> ResampleFilterType;
    typedef itk::CastImageFilter<InputImageType, OutputImageType> CastFilterType;
    typedef itk::ImageFileWriter<OutputImageType> WriterType;

    const static int COMPOSE_NONE   = 1000;
    const static int COMPOSE_POST   = 1001;
    const static int COMPOSE_PRE    = 1002;

    void SetSource(FileSet* source);
    void SetTransforms(TransformGroup::TransformVector* newTransforms);
    void SetDestination(FileSet* destination);
    void Update(void);
    void SetComposeMode(int mode);
private:
    FileSet* source;
    TransformGroup::TransformVector* transforms;
    FileSet* destination;
    int composeMode;
};
