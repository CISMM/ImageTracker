#pragma once

#include "itkAdaptImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"

enum ScalarPixelType
{
    CharPixel,
    ShortPixel,
    FloatPixel
};

template < class TInputPixel, class TOutputPixel >
class PixelCastAccessor
{
public:        
    typedef TInputPixel InternalType;
    typedef TOutputPixel ExternalType;
    
    ExternalType Get(const InternalType& input)
    { return static_cast<ExternalType>(input); }
};

typedef itk::Image< unsigned char, 2 > ImageTypeUC2;
typedef itk::Image< unsigned short, 2 > ImageTypeUS2;
typedef itk::Image< float, 2 > ImageTypeF2;
typedef ImageTypeF2 ImageType;

typedef itk::ImageFileReader< ImageTypeUC2 > ReaderTypeUC2;
typedef itk::ImageFileReader< ImageTypeUS2 > ReaderTypeUS2;
typedef itk::ImageFileReader< ImageTypeF2 > ReaderTypeF2;

typedef itk::AdaptImageFilter< ImageTypeUC2, ImageType,
    PixelCastAccessor< ImageTypeUC2::PixelType, ImageType::PixelType > > CasterTypeUC2;
typedef itk::AdaptImageFilter< ImageTypeUS2, ImageType,
    PixelCastAccessor< ImageTypeUS2::PixelType, ImageType::PixelType > > CasterTypeUS2;
typedef itk::AdaptImageFilter< ImageTypeF2, ImageType,
    PixelCastAccessor< ImageTypeF2::PixelType, ImageType::PixelType > > CasterTypeF2;

