#pragma once

#include "itkImage.h"
#include "itkVector.h"

/**
 * \class VectorFileSet
 * \brief Defines operations for handling sets of vector images.
 * 
 * Abstract interface that defines the key functions for handling a 
 * set of vector images.  The method of file retrieval is not
 * specified--the output could come from a direct read from disk
 * or from a series of vector image filters.  This interface is meant 
 * to flexibility in handing sets of vector images in a generic
 * manner, whether in a batch processing task or in an interactive
 * application.
 * 
 * VectorFileSet always provides vector images of type
 * VectorFileSet::ImageType.
 */
class VectorFileSet
{
public:
    typedef itk::Vector<float, 2> PixelType;
    typedef itk::Image< PixelType, 2> ImageType;
    
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
     * Retrieve the current vector image from the VectorFileSet.
     * Note that subsequent calls to GetOutput may invalidate the
     * returned image data.  Use an itk::ImageDuplicator on the
     * output to isolate the data.
     */
    virtual ImageType* GetOutput() = 0;
    
    /**
     * Retrieve a vector image with the given index from the 
     * VectorFileSet. Note that subsequent calls to GetOutput may 
     * invalidate the returned image data.  Use an 
     * itk::ImageDuplicator on the output to isolate the data.
     */
    virtual ImageType* GetImage(unsigned int index) = 0;
    
    /**
     * Get the number of vector images in this VectorFileSet.
     */
    virtual int GetImageCount() = 0;
};
