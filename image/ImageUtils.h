#pragma once

#include <string>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkStatisticsImageFilter.h"

#include "Logger.h"

template < class TImage >
void PrintImageInfo(const TImage* image, const std::string& label = "", LogStream &logger = Logger::debug)
{
    typedef itk::StatisticsImageFilter< TImage> StatsType;

    typename StatsType::Pointer stats = StatsType::New();
    stats->SetInput(image);
    stats->Update();

    if (label != "")
        logger << label << " info:" << std::endl;
    else
        logger << "Image info: " << std::endl;
    
    logger << "\tclass: " << image->GetNameOfClass() << std::endl;
    logger << "\tmin:   " << (float) stats->GetMinimum() << std::endl;
    logger << "\tmax:   " << (float) stats->GetMaximum() << std::endl;
    logger << "\tmean:  " << (float) stats->GetMean() << std::endl;
    logger << "\tvar:   " << (float) stats->GetVariance() << std::endl;
    logger << "\tstd:   " << (float) stats->GetSigma() << std::endl;
}

template < class TImage >
void PrintRegionInfo(const typename TImage::RegionType& region, const std::string& label = "", LogStream &logger = Logger::debug)
{
    unsigned int Dimension = TImage::ImageDimension;
    if (label != "")
        logger << label << " info: " << std::endl;
    else
        logger << "Region info: " << std::endl;
    
    // We have to be deterministic (we can't just do this in a for loop) here, to avoid odd printing of log levels
    if (Dimension == 2)
    {
        logger << "\tIndex:\t[" << region.GetIndex()[0] << "," << region.GetIndex()[1] << "]" << std::endl;
        logger << "\tSize:\t["  << region.GetSize()[0]  << "," << region.GetSize()[1]  << "]" << std::endl;
    }
    else if (Dimension == 3)
    {
        logger << "\tIndex:\t[" << region.GetIndex()[0] << "," << region.GetIndex()[1] << "," << region.GetIndex()[2] << "]" << std::endl;
        logger << "\tSize:\t["  << region.GetSize()[0]  << "," << region.GetSize()[1] << ","  << region.GetSize()[2]  << "]" << std::endl;
    }
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

template < class TImageIn, class TImageOut >
void WriteImage(const TImageIn* image, const std::string& filename, bool rescale = true)
{
    Logger::verbose << "Writing:\t" << filename << std::endl;
    typedef itk::RescaleIntensityImageFilter< TImageIn, TImageOut > CastType;
    typedef itk::ImageFileWriter< TImageOut > WriterType;
    typedef typename TImageOut::PixelType OutputPixelType;
    
    typename CastType::Pointer caster = CastType::New();
    typename WriterType::Pointer writer = WriterType::New();
    caster->SetInput(image);
    writer->SetInput(caster->GetOutput());
    writer->SetFileName(filename.c_str());
    if (rescale)
    {
        caster->SetOutputMinimum(std::numeric_limits<OutputPixelType>::min());
        caster->SetOutputMaximum(std::numeric_limits<OutputPixelType>::max());
    }
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
