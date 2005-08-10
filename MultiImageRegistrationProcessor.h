#pragma once

#include <vector>

#include "CommonTypes.h"
#include "FileSet.h"
#include "ImageRegistration.h"

/**
 * Handles registration (alignment) of multiple images, such as those in a 
 * video sequence.  
 */
class MultiImageRegistrationProcessor
{
public:
    MultiImageRegistrationProcessor(void);
    ~MultiImageRegistrationProcessor(void);

    enum{ALIGN_WITH_FIRST       = 1000,  ALIGN_WITH_PREVIOUS    = 1001};
    typedef std::vector<CommonTypes::TransformPointer> TransformVector;

    /*
     * Sets up this registration processor's image source.
     */
    void SetSource(FileSet* source);

    /*
     * Sets up the object used for image registration for this registration
     * processor.
     */
    void SetImageRegistration(ImageRegistration* registrar);

    /*
     * Starts the multiple image registration process.  When this 
     * function returns, the set of image transformations from the
     * multiple image registration process will be available.
     */
    TransformVector* Update(void);

    /*
     * Retrieves the group of transformations created by this image registration
     * processor.  If the image registration process has not been completed,
     * this function will not return a set of transformations.
     */
    TransformVector* GetTransforms(void);

    /**
     * Sets the alignment mode used for multiple image processing.  For example,
     * ALIGN_WITH_FIRST will set up the registration processor to align each image
     * with the first image in the processed sequence.  ALIGN_WITH_PREVIOUS will 
     * align each image with the image occuring previously in the sequence.
     */
    void SetAlignmentMode(int mode);

private:
    FileSet* source;
    ImageRegistration* registrar;
    TransformVector* transforms;
    bool registrationComplete;
    int alignMode;
};
