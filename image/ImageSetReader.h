#pragma once

#include <algorithm>
#include <deque>
#include <string>
#include <vector>

#include "itkAdaptImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkDataObject.h"
#include "itkImageFileReader.h"
#include "itkLightObject.h"

#include "FileSet.h"
#include "ImageUtils.h"
#include "Logger.h"

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

/**
 * An ImageSetReader provides an intuitive interface to a set of
 * images, e.g. frames of a video. The class attepts to mirror the
 * familiar standard template library (STL) collection interface. The
 * memory management may differ from a STL implementation, however,
 * as not all images need to be loaded into memory at once.
 * Additionally, loaded images may be removed from memory.
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
    typedef itk::AdaptImageFilter< InputImageType, OutputImageType, 
        PixelAccessor< typename InputImageType::PixelType, typename OutputImageType::PixelType > > CasterType;
//     typedef itk::CastImageFilter< InputImageType, OutputImageType > CasterType;
    typedef typename CasterType::Pointer CasterPointer;

    // We have to store references to the image caster--otherwise it gets deleted.
    typedef std::vector<CasterPointer> ImageArray;
    typedef std::deque<unsigned int> IndexArray;
    
    /**
     * Default constructor.
     */
    ImageSetReader() :
        Superclass(),
        indices(),
        requestCount(0),
        readCount(0),
        maxCount(0)
    {}

    /**
     * Construct an ImageSetReader from a set of files.
     */
    ImageSetReader(const FileSet& set) :
        Superclass(),
        indices(),
        requestCount(0),
        readCount(0),
        maxCount(0)
    {
        this->SetFiles(set);
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

private:
    /*
     * We use a deque to keep track of the indices of images
     * that are loaded in memory.  Deques are fast for inserting
     * and removing elements at either end.
     */
    IndexArray indices;

    /*
     * We use a vector of image reader pointers to store the images
     * we have loaded.
     */
    ImageArray images;

    int requestCount;
    int readCount;
    int maxCount;
};

// Implementation //

template < class TInput, class TOutput >
typename ImageSetReader<TInput, TOutput>::OutputImagePointer
ImageSetReader<TInput, TOutput>::operator[](unsigned int index)
{
    // Logger::verbose << "ImageSetReader[" << index << "] begin." << std::endl;
    this->requestCount++;
    // See if the index is in the loaded image list
    IndexArray::iterator idx = find(this->indices.begin(), this->indices.end(), index);

    if (idx == this->indices.end()) // the requested index is not loaded
    {
        // Logger::debug << "Image[" << index << "]: " << this->files.GetFileNames()[index] << std::endl;
        this->readCount++;
        // Read the file
        CasterPointer caster = CasterType::New();
        caster->SetInput(ReadImage<InputImageType>(const_cast<FileSet&>(this->GetFiles())[index]));
        this->images[index] = caster;
//         this->images[index] = ReadImage<InputImageType>(const_cast<FileSet&>(this->GetFiles())[index]);

        this->indices.push_back(index);
        // must count stored indices because the image array's size is
        // the same as that of the file list.
        this->maxCount = this->indices.size() > this->maxCount ?
                this->indices.size() : this->maxCount;
    }

    
    // Logger::verbose << "ImageSetReader[" << index << "] end." << std::endl;
    return this->images[index]->GetOutput();
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
    this->indices.clear();
    this->images.clear();
    // Note on semantics:
    // images.resize() is the proper method to use here--
    // it fills in all elements of the images vector with a default
    // image pointer.
    // images.reserve() would only allocate the proper amount
    // of memory, but the vector elements would not be initialized.
    // This leads to general badness when trying to assign values
    // to elements in the vector.
    this->images.resize(this->size());
}
