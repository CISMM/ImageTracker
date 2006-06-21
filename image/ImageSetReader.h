#pragma once

#include <algorithm>
#include <deque>
#include <string>
#include <vector>

#include "itkCastImageFilter.h"

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

    typedef std::vector<InputImagePointer> ImageVector;
    typedef std::deque<unsigned int> IndexList;

    /**
     * Construct an ImageSetReader from a set of files.
     */
    ImageSetReader(const FileSet& set) :
        files(set),
        requestCount(0),
        readCount(0),
        maxCount(0)
    {
        this->images.reserve(set.size());
    }

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
    IndexList indices;

    /*
     * We use a vector of image pointers to store the images
     * we have loaded.
     */
    ImageVector images;

    int requestCount;
    int readCount;
    int maxCount;
};

// Implementation //

template < class TInput, class TOutput >
typename ImageSetReader<TInput, TOutput>::OutputImagePointer 
ImageSetReader<TInput, TOutput>::operator[](unsigned int index)
{
    this->requestCount++;
    // See if the index is in the loaded image list
    IndexList::iterator idx = find(this->indices.begin(), this->indices.end(), index);

    if (idx == this->indices.end()) // the requested index is not loaded
    {
        // Logger::debug << "Image[" << index << "]: " << this->files.GetFileNames()[index] << std::endl;
        this->readCount++;
        this->images[index] = ReadImage< InputImageType >(this->files.GetFileNames()[index]);
        this->indices.push_back(index);
        this->maxCount = this->indices.size() > this->maxCount ? 
            this->indices.size() : this->maxCount;
    }

    typename CasterType::Pointer caster = CasterType::New();
    caster->SetInput(this->images[index]);
    caster->Update();
    return caster->GetOutput();
}
