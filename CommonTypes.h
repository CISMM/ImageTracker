#pragma once

#include "itkImage.h"
#include "itkCenteredRigid2DTransform.h"

/**
 * Defines common types used for registration.
 */
class CommonTypes
{
public:
    const static int Dimension = 2;
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