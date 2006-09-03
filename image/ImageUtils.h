#pragma once

#include <string>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkStatisticsImageFilter.h"

#include "Logger.h"

template < class TImage >
void PrintImageInfo(const TImage* image, const std::string& label = "")
{
    typedef itk::StatisticsImageFilter< TImage> StatsType;

    typename StatsType::Pointer stats = StatsType::New();
    stats->SetInput(image);
    stats->Update();

    if (label != "")
        std::cout << label << " info:" << std::endl;
    else
        std::cout << "Image info: " << std::endl;
    std::cout << "\tclass: " << image->GetNameOfClass() << std::endl;
    std::cout << "\tmin:   " << (float) stats->GetMinimum() << std::endl;
    std::cout << "\tmax:   " << (float) stats->GetMaximum() << std::endl;
    std::cout << "\tmean:  " << (float) stats->GetMean() << std::endl;
    std::cout << "\tvar:   " << (float) stats->GetVariance() << std::endl;
    std::cout << "\tstd:   " << (float) stats->GetSigma() << std::endl;
}

template < class TImage >
void WriteImage(const TImage* image, const std::string& filename)
{
    Logger::verbose << "Writing:\t" << filename << std::endl;
    typedef itk::ImageFileWriter< TImage > WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetInput(image);
    writer->SetFileName(filename.c_str());
    writer->Update();
}

template < class TImage >
typename TImage::Pointer ReadImage(const std::string& filename)
{
    Logger::verbose << "Reading:\t" << filename << std::endl;
    typedef itk::ImageFileReader< TImage > ReaderType;
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(filename.c_str());
    reader->Update();
    return reader->GetOutput();
}

template <class TSourceImage, class TDestImage >
void CopyRegion(const typename TSourceImage::RegionType& source, typename TDestImage::RegionType& destination)
{
    const unsigned int dimension = TSourceImage::ImageDimension;

    typename TDestImage::RegionType::SizeType size;
    typename TDestImage::RegionType::IndexType index;

    for (unsigned int d = 0; d < dimension; d++)
    {
        size[d] = source.GetSize()[d];
        index[d] = source.GetIndex()[d];
    }

    destination.SetSize(size);
    destination.SetIndex(index);
}
