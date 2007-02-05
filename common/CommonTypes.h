#pragma once

#include "itkImage.h"
#include "itkCenteredRigid2DTransform.h"

typedef itk::Image< unsigned char, 2 > ImageTypeUC2;
typedef itk::Image< unsigned short, 2 > ImageTypeUS2;
typedef itk::Image< float, 2 > ImageTypeF2;
typedef itk::Image< itk::Vector< float, 2 >, 2 > ImageTypeV2F2;

/**
 * Defines common types used for registration.
 */
class CommonTypes
{
public:
    enum { Dimension = 2};
    typedef unsigned short InputPixelType;
    typedef itk::Image<InputPixelType, Dimension> InputImageType;
    
    typedef float InternalPixelType;
    typedef itk::Image<InternalPixelType, Dimension> InternalImageType;

    typedef unsigned char DisplayPixelType;
    typedef itk::Image<DisplayPixelType, Dimension> DisplayImageType;

    typedef unsigned char OutputPixelType;
    typedef itk::Image<OutputPixelType, Dimension> OutputImageType;
    
    typedef InternalImageType ImageType;
    typedef ImageType::Pointer ImagePointer;

    typedef itk::CenteredRigid2DTransform<double> TransformType;
    typedef TransformType::Pointer TransformPointer;
protected:
private:
};

