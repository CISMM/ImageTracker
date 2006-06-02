#pragma once

#include <string>

#include "itkStatisticsImageFilter.h"
#include "itkImageFileWriter.h"

template < class TImage >
void PrintImageInfo( const TImage* image, const std::string& label = "")
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

template < class TImage>
void SaveImage( const TImage* image, const std::string& filename)
{
	typedef itk::ImageFileWriter< TImage > WriterType;
	typename WriterType::Pointer writer = WriterType::New();
	writer->SetInput(image);
	writer->SetFileName(filename.c_str());
	writer->Update();
	std::cout << "Wrote: " << filename << std::endl;
}