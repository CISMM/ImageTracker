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

/*
 * Handles output from image registration.  Creates a set of transformed images
 * from an input file set and a set of transforms.  Transforms for a given registration
 * can be composed (pre-composed or post-composed).
 */
class RegistrationOutput
{
public:
    /*
     * Complete constructor.  Requires a set of source files, a set of transforms, a set of destination files, and a compose mode.
     */
    RegistrationOutput(FileSet* source = NULL, TransformGroup::TransformVector* transforms = NULL, FileSet* destination = NULL, int composeMode = RegistrationOutput::COMPOSE_PRE );
    /*
     * Desctructor.
     */
    ~RegistrationOutput(void);

    typedef CommonTypes::OutputImageType OutputImageType;
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
    
    /*
     * Save the result of transforming all source image files to destination image files.
     */
    void Update(void);

    /*
     * Specifies a compose mode for the transform group.  
     * COMPOSE_NONE means no transform composition.
     * COMPOSE_POST means transform post-composition.
     * COMPOSE_PRE means transform pre-composition.
     */
    void SetComposeMode(int mode);

private:
    FileSet* source;
    TransformGroup::TransformVector* transforms;
    FileSet* destination;
    int composeMode;
};
