#pragma once

#include <string>

#include "itkCastImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkStatisticsImageFilter.h"
#include "itkVector.h"

#include "CommonTypes.h"
#include "Logger.h"

template < class TImage >
void PrintImageInfo(const TImage* image, const std::string& label = "", LogStream &logger = Logger::debug)
{
    if (label != "")
        logger << label << " info:" << std::endl;
    else
        logger << "Image info: " << std::endl;
    
    unsigned int dim = TImage::ImageDimension;
    typename TImage::RegionType region = image->GetLargestPossibleRegion();
    typename TImage::PointType origin = image->GetOrigin();
    typename TImage::SpacingType spacing = image->GetSpacing();
    if (dim == 3)
    {
        logger << "\tOrigin: \t" << origin[0] << ", " << origin[1] << ", " << origin[2] << std::endl;
        logger << "\tSpacing:\t" << spacing[0] << ", " << spacing[1] << ", " << spacing[2] << std::endl;
        logger << "\tRegion: \t" << std::endl;
        logger << "\t\tIndex:\t" << region.GetIndex()[0] << ", " << region.GetIndex()[1] << ", " << region.GetIndex()[2] << std::endl;
        logger << "\t\tSize: \t" << region.GetSize()[0] << ", " << region.GetSize()[1] << ", " << region.GetSize()[2] << std::endl;
    }
    else if (dim == 2)
    {
        logger << "\tOrigin: \t" << origin[0] << ", " << origin[1] << std::endl;
        logger << "\tSpacing:\t" << spacing[0] << ", " << spacing[1] << std::endl;
        logger << "\tRegion: \t" << std::endl;
        logger << "\t\tIndex:\t" << region.GetIndex()[0] << ", " << region.GetIndex()[1] << std::endl;
        logger << "\t\tSize: \t" << region.GetSize()[0] << ", " << region.GetSize()[1] << std::endl;
    }
    else if (dim == 1)
    {
        logger << "\tOrigin: \t" << origin[0] << std::endl;
        logger << "\tSpacing:\t" << spacing[0] << std::endl;
        logger << "\tRegion: \t" << std::endl;
        logger << "\t\tIndex:\t" << region.GetIndex()[0] << std::endl;
        logger << "\t\tSize: \t" << region.GetSize()[0] << std::endl;
    }
    
    typedef itk::StatisticsImageFilter< TImage > StatsType;
    typename StatsType::Pointer stats = StatsType::New();
    stats->SetInput(image);
    stats->Update();
    
    logger << "\tclass:\t" << image->GetNameOfClass() << std::endl;
    logger << "\tmin:  \t" << (float) stats->GetMinimum() << std::endl;
    logger << "\tmax:  \t" << (float) stats->GetMaximum() << std::endl;
    logger << "\tmean: \t" << (float) stats->GetMean() << std::endl;
    logger << "\tvar:  \t" << (float) stats->GetVariance() << std::endl;
    logger << "\tstd:  \t" << (float) stats->GetSigma() << std::endl;
}

template <>
void PrintImageInfo< ImageTypeV2F2 >(const ImageTypeV2F2* image, const std::string& label, LogStream &logger);

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
    typedef itk::RescaleIntensityImageFilter< TImageIn, TImageOut > RescaleType;
    typedef itk::CastImageFilter< TImageIn, TImageOut > CastType;
    typedef itk::ImageFileWriter< TImageOut > WriterType;
    typedef typename TImageOut::PixelType OutputPixelType;
    
    // We set up two pipelines, but we only execute one
    typename CastType::Pointer caster = CastType::New();
    typename RescaleType::Pointer rescaler = RescaleType::New();
    typename WriterType::Pointer writer = WriterType::New();
    caster->SetInput(image);
    rescaler->SetInput(image);
    writer->SetFileName(filename.c_str());
    
    rescaler->SetOutputMinimum(std::numeric_limits<OutputPixelType>::min());
    rescaler->SetOutputMaximum(std::numeric_limits<OutputPixelType>::max());
    
    if (rescale) // here we choose whether to rescale, or just cast
    {
        writer->SetInput(rescaler->GetOutput());
    }
    else
    {
        writer->SetInput(caster->GetOutput());
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

template <class TRegionType>
TRegionType PadRegionByRadius(const TRegionType& region, int radius)
{
    const unsigned int dimension = TRegionType::GetImageDimension();
    
    typename TRegionType::SizeType size;
    typename TRegionType::IndexType index;
    
    for (unsigned int d = 0; d < dimension; d++)
    {
        index[d] = region.GetIndex()[d] - radius;
        
        // have to be careful if radius is negative...check to make sure we don't
        // end up with a negative size
        if ((int)region.GetSize()[d] + 2*radius < 0)
        {
            size[d] = region.GetSize()[d];
        }
        else
        {
            size[d] = region.GetSize()[d] + 2*radius;
        }
    }
    
    TRegionType regionOut;
    regionOut.SetSize(size);
    regionOut.SetIndex(index);
    return regionOut;
}
