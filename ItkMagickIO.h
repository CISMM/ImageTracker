#pragma once
#include <string>

#include "itkCastImageFilter.h"
#include "itkImage.h"
#include "Magick++.h"

#include "CommonTypes.h"

class ItkMagickIO
{
public:
    typedef CommonTypes::InternalImageType InternalImageType;
    // A Magick::Quantum is one byte; but there are four of those for each pixel
    typedef unsigned short MagickPixelType;
    const static int Dimension = CommonTypes::Dimension;
    typedef itk::Image<MagickPixelType, Dimension> MagickImageType;
    typedef itk::CastImageFilter<MagickImageType, InternalImageType> CasterType;

    /*
     * Convert image data from Magick::Image to itk::Image.
     */
    static InternalImageType::Pointer MagickToItk(Magick::Image magImage);
    static Magick::Image ItkToMagick(InternalImageType::Pointer itkImage);
    static InternalImageType::Pointer Read(std::string filename);
    static void Write(std::string filename, InternalImageType::Pointer itkImage);
};
