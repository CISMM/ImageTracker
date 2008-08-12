#pragma once

#include "ImageFileSet.h"
#include "FilterControlPanel.h"

/**
 * \class CompositeImageFileSet
 * \brief Presents the result of possibly several FilterControls as an ImageFileSet.
 * 
 * This class is a convenience wrapper that presents the result of compositing several
 * filters as an ImageFileSet.  It is very specific to the ImageTracker live filter
 * architecture.  This ImageFileSet can be used as the input to an ItkImagePipeline,
 * which means the pipeline can operate on the entire image set without needing to know what
 * filters are present in the pipeline.
 */
class CompositeImageFileSet
    : public ImageFileSet
{
public:   
    CompositeImageFileSet();
    virtual ~CompositeImageFileSet(); 
    
    /**
     * Set the start of the pipeline for this ImageFileSet. The
     * start needs to be an ImageFileSet to support changing the
     * image index and determining the image count.
     */
    void SetStart(ImageFileSet* start);
    /**
     * Set the end of the image pipeline.  This needs to be a 
     * FilterControlPanel that is connected to the start of the
     * pipeline.
     */
    void SetEnd(FilterControlPanel* end);
    
    virtual void SetImageIndex(unsigned int index);
    virtual unsigned int GetImageIndex();    
    virtual ImageFileSet::ImageType* GetOutput();
    virtual ImageFileSet::ImageType* GetImage(unsigned int index);
    virtual int GetImageCount();
    
protected:
    
private:
    // Not implemented
    CompositeImageFileSet(const CompositeImageFileSet& other);
    void operator=(const CompositeImageFileSet& other);
    
    ImageFileSet* start;
    FilterControlPanel* end;
};
