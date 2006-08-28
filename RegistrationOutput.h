#pragma once

#include <string>

#include "itkCastImageFilter.h"
#include "itkImage.h"
#include "itkResampleImageFilter.h"

#include "CommonTypes.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "TransformGroup.h"

/*
 * Handles output from image registration.  Creates a set of transformed images
 * from an input file set and a set of transforms.  Transforms for a given registration
 * can be composed (pre-composed or post-composed).
 */
class RegistrationOutput
{
public:
    
    RegistrationOutput() :
        composeMode(RegistrationOutput::COMPOSE_PRE)
    {}
    
    /*
     * Complete constructor.  Requires a set of source files, a set of transforms, a set of destination files, and a compose mode.
     */
    RegistrationOutput(const FileSet& source, const TransformGroup::TransformVector& transforms, const FileSet& destination, int composeMode = RegistrationOutput::COMPOSE_PRE ):
        source(source),
        destination(destination),
        transforms(transforms),
        composeMode(composeMode)
    {}
    
    /*
     * Desctructor.
     */
    virtual ~RegistrationOutput(void) {}

    typedef CommonTypes::OutputImageType OutputImageType;
    typedef CommonTypes::ImageType InputImageType;
    typedef ImageSetReader<InputImageType> ReaderType;
   
    typedef itk::ResampleImageFilter<InputImageType, InputImageType> ResampleFilterType;
    typedef itk::CastImageFilter<InputImageType, OutputImageType> CastFilterType;

    enum {COMPOSE_NONE   = 1000,  COMPOSE_POST   = 1001, COMPOSE_PRE    = 1002};

    void SetSource(const FileSet& source) { this->source = source; }
    void SetDestination(const FileSet& destination) { this->destination = destination; }
    void SetTransforms(const TransformGroup::TransformVector& trans) {this->transforms = trans; }
    
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
    void SetComposeMode(int mode)
    {
        this->composeMode = mode;
    }

private:
    FileSet source;
    FileSet destination;
    TransformGroup::TransformVector transforms;
    int composeMode;
};
