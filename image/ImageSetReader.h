#pragma once

#include <algorithm>
#include <deque>
#include <string>

#include "itkAdaptImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkDataObject.h"
#include "itkImageFileReader.h"
#include "itkLightObject.h"
#include "itkVector.h"

#include "FileSet.h"
#include "ImageUtils.h"
#include "Logger.h"

/**
 * Base class for reading a set of images.  Defines generic methods all ImageSetReaders must implement,
 * such as image access and file name getting/setting.
 */
class ImageSetReaderBase
{
public:
    typedef ImageSetReaderBase Self;
    typedef itk::LightObject Superclass;
    
    /**
     * Default constructor.
     */
    ImageSetReaderBase() :
        files()
    {}
    
    virtual ~ImageSetReaderBase() {}
    
    virtual itk::DataObject* GetImage(unsigned int index)
    {
        // TBD: the linker complains if this method is left pure virtual,
        // I think because the compiler isn't able to create dynamic cast
        // information, which we use to determine what type of pixel type
        // a ImageSetReader can handle.
        Logger::warning << "ImageSetReaderBase::GetImage() called...this is bad because this function is not implemented." << std::endl;
        return NULL;
    }
    
    const FileSet& GetFiles() const
    { return this->files; }
    
    virtual void SetFiles(const FileSet& files)
    { this->files = files; }
    
    int size()
    { return this->files.size(); }
    
    /**
     * Assignment copy. Target grabs other image set's file list.
     */
    ImageSetReaderBase& operator=(const ImageSetReaderBase& other)
    {
        if (this != &other)	// test for self-assignment
        {
            // Basically, assignment has the effect of grabbing the other
            // image set's file list.
            this->SetFiles(other.GetFiles());
        }
        
        return *this;
    }
    
protected:
private:
    // The set of filenames behind this ImageSetReader.
    FileSet files;
};

template < class TInputPixel, class TOutputPixel >
class PixelAccessor
{
public:
    typedef TInputPixel InternalType;
    typedef TOutputPixel ExternalType;
    
    ExternalType Get(const InternalType& input)
    { return static_cast<ExternalType>(input); }
};

template <>
class PixelAccessor< itk::Vector< float, 2 >, itk::Vector< float, 2 > >
{
public:
    typedef itk::Vector< float, 2 > InternalType;
    typedef itk::Vector< float, 2 > ExternalType;
        
    ExternalType Get(const InternalType& input)
    {
        return input;
    }
};

/**
 * An ImageSetReader provides an intuitive interface to a set of
 * images, e.g. frames of a video. The class attepts to mirror the
 * familiar standard template library (STL) collection interface. The
 * memory management may differ from a STL implementation, however,
 * as not all images need to be loaded into memory at once.
 * Additionally, loaded images may be removed from memory.
 *
 * Specifically, one may change the memory management behavior of
 * an ImageSetReader by setting the CacheRadius; this controls how
 * many images are loaded in front of and behind the most recently
 * accessed image.
 */
template < class TInput, class TOutput = TInput >
class ImageSetReader : 
        public ImageSetReaderBase
{
public:
    typedef ImageSetReader Self;
    typedef ImageSetReaderBase Superclass;
    
    typedef TInput  InputImageType;
    typedef TOutput OutputImageType;
    typedef typename InputImageType::Pointer InputImagePointer;
    typedef typename OutputImageType::Pointer OutputImagePointer;
    // typedef itk::CastImageFilter< InputImageType, OutputImageType > CasterType;
    typedef itk::AdaptImageFilter< InputImageType, OutputImageType, 
        PixelAccessor< typename InputImageType::PixelType, typename OutputImageType::PixelType > > CasterType;
    typedef typename CasterType::Pointer CasterPointer;

    // We have to store references to the image caster--otherwise it gets deleted.
    typedef std::deque<CasterPointer> ImageArray;
    
    /**
     * Default constructor.
     */
    ImageSetReader() :
        Superclass(),
        images(),
        requestCount(0),
        readCount(0),
        maxCount(0),
        m_CacheRadius(0),
        m_Index(-1),
        m_MinIndex(-1),
        m_MaxIndex(-1)
    {}

    /**
     * Construct an ImageSetReader from a set of files.
     */
    ImageSetReader(const FileSet& set) :
        Superclass(),
        images(),
        requestCount(0),
        readCount(0),
        maxCount(0),
        m_CacheRadius(0),
        m_Index(-1),
        m_MinIndex(-1),
        m_MaxIndex(-1)
    {
        this->SetFiles(set);
    }
    
    virtual ~ImageSetReader()
    {
        Logger::debug << "ImageSetReader::~ImageSetReader()" << std::endl;
        this->LogStatistics();
    }

    /**
     * Access an image from this ImageSetReader by index.
     */
    OutputImagePointer operator[](unsigned int index);
    
    virtual itk::DataObject* GetImage(unsigned int index);

    /**
     * Change the files that this ImageSetReader refers to.
     */
    virtual void SetFiles(const FileSet& files);

    /**
     * Log access statistics.
     */
    void LogStatistics()
    {
        Logger::info << "ImageSetReader statistics" << std::endl;
        Logger::info << "\tRequests:\t" << this->requestCount << std::endl;
        Logger::info << "\tReads:   \t" << this->readCount << std::endl;
        Logger::info << "\tMax:     \t" << this->maxCount << std::endl;
    }
    
    /**
     * Get/Set the cache radius used by this ImageSetReader.  The cache radius
     * determines how many images will be kept in memory concurrently.
     */
    unsigned int GetCacheRadius() 
    { return this->m_CacheRadius; }
    void SetCacheRadius(unsigned int radius);

private:
    
    /**
     * Load the image with the given index from disk.
     */
    CasterPointer LoadImage(unsigned int index);

    /*
     * We use a vector of image reader pointers to store the images
     * we have loaded.
     */
    ImageArray images;

    // Reader statistics variables
    int requestCount;
    int readCount;
    int maxCount;
    
    // Image cache managment variables
    unsigned int m_CacheRadius;
    int m_Index;
    int m_MinIndex;
    int m_MaxIndex;
};

// Implementation //

template < class TInput, class TOutput >
typename ImageSetReader<TInput, TOutput>::OutputImagePointer
ImageSetReader<TInput, TOutput>::operator[](unsigned int idx)
{
    std::string function("ImageSetReader::[]");
//     Logger::verbose << function << ": [" << idx << "] begin." << std::endl;
    this->requestCount++;
    
    // Ensure the requested index is valid
    // Doing this ensures we always return an image--the last one, if the requested index is too high.
    idx = std::min((int) idx, this->size()-1);
    
    // See how far the requested index is from the current index
    int dIdx = idx - this->m_Index;
    // Compute what the min and max index held in the cache should be
    int minIndex = std::max(0, (int) (idx - this->GetCacheRadius()));
    int maxIndex = std::min(this->size()-1, (int) (idx + this->GetCacheRadius()));
//     Logger::verbose << function 
//             << "\n\tthis->m_Index    " << this->m_Index 
//             << "\n\tthis->m_MinIndex " << this->m_MinIndex
//             << "\n\tthis->m_MaxIndex " << this->m_MaxIndex
//             << "\n\tnew index        " << idx
//             << "\n\tnew MinIndex     " << minIndex
//             << "\n\tnew MaxIndex     " << maxIndex 
//             << "\n\timage cache size " << this->images.size() << std::endl;
    
    if (abs(dIdx) <= this->GetCacheRadius() + 1)
    {
        if (dIdx > 0)
        {
            // The requested index is greater than the current index
            // and the new index is within one cache radius of the old index
//             Logger::verbose << function << ": stepping forward through image list" << std::endl;
            for (int i = this->m_MinIndex; i < minIndex && i >= 0; i++)
            {
//                 Logger::verbose << function << ": popping image off the front" << std::endl;
                this->images.pop_front();
            }
            for (int i = this->m_MaxIndex + 1; i <= maxIndex; i++)
            {
//                 Logger::verbose << function << ": pushing image onto the back" << std::endl;
                this->images.push_back(this->LoadImage(i));
            }
        }
        else if (dIdx < 0)
        {
            // The requested index is less than the current index
            // and the new index is within one cache radius of the old index
//             Logger::verbose << function << ": stepping backwards through image list" << std::endl;
            for (int i = this->m_MaxIndex; i > maxIndex; i--)
            {
//                 Logger::verbose << function << ": popping image off the back" << std::endl;
                this->images.pop_back();
            }
            for (int i = this->m_MinIndex - 1; i >= minIndex; i--)
            {
//                 Logger::verbose << function << ": pushing image onto the front" << std::endl;
                this->images.push_front(this->LoadImage(i));
            }
        }
    }
    else
    {
        // The requested index is nowhere near the current index
        // Discard the whole cacha and start from scratch
//         Logger::verbose << function << ": discarding image cache" << std::endl;
        this->images.clear();
        for (int i = minIndex; i <= maxIndex; i++)
        {
            this->images.push_front(this->LoadImage(i));
        }
    }
    
    this->m_Index = idx;
    this->m_MinIndex = minIndex;
    this->m_MaxIndex = maxIndex;
    this->maxCount = std::max(this->maxCount, (int) this->images.size());
    
//     Logger::verbose << function << ": image cache size " << this->images.size() << std::endl;
//     Logger::verbose << function << ": returning image at index " << (this->m_Index - this->m_MinIndex) << std::endl;
    return this->images[this->m_Index - this->m_MinIndex]->GetOutput();
}

template < class TInput, class TOutput >
typename ImageSetReader<TInput, TOutput>::CasterPointer
ImageSetReader<TInput, TOutput>::LoadImage(unsigned int index)
{
    this->readCount++;
    
    // Cast the image to the output image type
    CasterPointer cast = CasterType::New();
    cast->SetInput(ReadImage<InputImageType>(this->GetFiles()[index]));
    cast->Update();
    return cast;
}

template < class TInput, class TOutput >
void ImageSetReader<TInput, TOutput>::SetCacheRadius(unsigned int radius)
{
    this->m_CacheRadius = radius;
    this->images.clear();
    
    // this will force a load on the next request
    this->m_Index = -1; 
    this->m_MinIndex = -1;
    this->m_MaxIndex = -1;
}

template < class TInput, class TOutput >
itk::DataObject*
ImageSetReader<TInput, TOutput>::GetImage(unsigned int index)
{
//     Logger::verbose << "ImageSetReader::GetImage(" << index << ")" << std::endl;
    return static_cast< itk::DataObject* >(this->operator[](index));
}

template < class TInput, class TOutput >
void ImageSetReader< TInput, TOutput >::SetFiles(const FileSet& files)
{
    Superclass::SetFiles(files);
    // We must be sure to make the images and indices to be valid.
    // For example, we need to ensure that enough space is reserved to
    // hold all image pointers and make sure no indices refer to old
    // files that should no longer be loaded by this ImageSetReader.
    this->images.clear();
    
    // this will force a load on the next request
    this->m_Index = -1;
    this->m_MinIndex = -1;
    this->m_MaxIndex = -1;
    // Note on semantics:
    // images.resize() fills in all elements of the images vector with
    // a default image pointer.
    // images.reserve() would only allocate the proper amount
    // of memory, but the vector elements would not be initialized.
    // This leads to general badness when trying to assign values
    // to elements in the vector.
//     this->images.resize(2 * this->GetCacheRadius() + 1);
}
