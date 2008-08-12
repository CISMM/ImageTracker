#pragma once

#include "itkImage.h"

/**
 * \class ImageFileSet
 * \brief Provides access to a set of image files.
 * 
 * Interface that defines the key functions required to access a
 * set of image files.  The method of file retrieval is not
 * specified--the output could come from a direct read from disk
 * or from a series of image filters.  This class is meant to 
 * provide more flexibility in handing sets of images in a generic
 * manner, whether in a batch processing task or in an interactive
 * application.
 * 
 * ImageFileSet always provides images of the type defined by
 * ImageFileSet::ImageType.
 */
class ImageFileSet
{
public:
    typedef itk::Image<float, 2> ImageType;
    
    /**
     * Set the index of the current image that will be retreived when
     * GetOutput() is called.
     */
    virtual void SetImageIndex(unsigned int index) = 0;
    
    /**
     * Get the index of the currently active image.
     */
    virtual unsigned int GetImageIndex() = 0;
    
    /**
     * Retrieve the current image from the ImageFileSet.  Note
     * that subsequent calls to GetOutput may invalidate the
     * returned image data.  Use an itk::ImageDuplicator on the
     * output to isolate the data.
     */
    virtual ImageType* GetOutput() = 0;
    
    /**
     * Retrieve an image with the given index from the ImageFileSet.
     * Note that subsequent calls to GetOutput may invalidate the
     * returned image data.  Use an itk::ImageDuplicator on the output
     * to isolate the data.
     */
    virtual ImageType* GetImage(unsigned int index) = 0;
    
    /**
     * Get the number of images in this ImageFileSet.
     */
    virtual int GetImageCount() = 0;
};
