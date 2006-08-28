#pragma once

#include <algorithm>
#include <deque>
#include <string>
#include <vector>

#include "itkCastImageFilter.h"
#include "itkImageFileReader.h"

#include "FileSet.h"
#include "ImageUtils.h"
#include "Logger.h"

/**
 * An ImageSetReader provides an intuitive interface to a set of
 * images, e.g. frames of a video. The class attepts to mirror the
 * familiar standard template library (STL) collection interface. The
 * memory management may differ from a STL implementation, however,
 * as not all images need to be loaded into memory at once.
 * Additionally, loaded images may be removed from memory.
 */
template < class TInput, class TOutput = TInput >
class ImageSetReader
{
public:
    typedef TInput  InputImageType;
    typedef TOutput OutputImageType;
    typedef typename InputImageType::Pointer InputImagePointer;
    typedef typename OutputImageType::Pointer OutputImagePointer;
    typedef itk::CastImageFilter< InputImageType, OutputImageType > CasterType;

	typedef std::vector<InputImagePointer> ImageArray;
    typedef std::deque<unsigned int> IndexArray;

    /**
     * Default constructor.
     */
    ImageSetReader() :
        requestCount(0),
        readCount(0),
        maxCount(0)
    {
    }

    /**
     * Construct an ImageSetReader from a set of files.
     */
    ImageSetReader(const FileSet& set) :
        files(set),
        requestCount(0),
        readCount(0),
        maxCount(0)
    {
		// Note on semantics: 
		// images.resize() is the proper method to use here--
		// it fills in all elements of the images vector with a default
		// image pointer.
		// images.reserve() would only allocate the proper amount
		// of memory, but the vector elements would not be initialized.
		// This leads to general badness when trying to assign values
		// to elements in the vector.
		this->images.resize(this->files.size());
    }

	/**
	 * Assignment copy.
	 */
	ImageSetReader& operator=(const ImageSetReader& other);

    /**
     * Returns the number of images behind this ImageSetReader.
     */
    unsigned int size() const 
    {
        return this->files.size();
    }

    /**
     * Access an image from this ImageSetReader by index.
     */
    OutputImagePointer operator[](unsigned int index);
    
	/**
	 * Change the files that this ImageSetReader refers to.
	 */
	void SetFiles(const FileSet& files);

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
    // The set of filenames behind this ImageSetReader.
    FileSet files;

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
		this->images[index] = ReadImage<InputImageType>(this->files[index]);

        this->indices.push_back(index);
		// must count stored indices because the image array's size is
		// the same as that of the file list.
        this->maxCount = this->indices.size() > this->maxCount ? 
            this->indices.size() : this->maxCount;
    }

    typename CasterType::Pointer caster = CasterType::New();
	caster->SetInput(this->images[index]);
    caster->Update();
    // Logger::verbose << "ImageSetReader[" << index << "] end." << std::endl;
    return caster->GetOutput();
}

template < class TInput, class TOutput >
ImageSetReader<TInput, TOutput>&
ImageSetReader<TInput, TOutput>::operator=(const ImageSetReader& other)
{
	if (this != &other)	// test for self-assignment
	{
		// Basically, assignment has the effect of grabbing the other
		// ImageSetReader's file list.
		this->SetFiles(other.files);
	}

	return *this;
}

template < class TInput, class TOutput >
void ImageSetReader< TInput, TOutput >::SetFiles(const FileSet& files)
{
	// We must be sure to make the images and indices to be valid.
	// For example, we need to ensure that enough space is reserved to
	// hold all image pointers and make sure no indices refer to old
	// files that should no longer be loaded by this ImageSetReader.
	this->files = files;
	this->indices.clear();
	this->images.clear();
	this->images.resize(this->files.size());
}
