#pragma once

#include "itkImage.h"
#include "itkCenteredRigid2DTransform.h"

/**
 * Defines common types used for registration.
 */
class CommonTypes
{
public:
    typedef double PixelType;
    const static int Dimension = 2;
    typedef itk::Image<PixelType, Dimension> ImageType;
    typedef ImageType::Pointer ImagePointer;

    typedef itk::CenteredRigid2DTransform<double> TransformType;
    typedef TransformType::Pointer TransformPointer;
protected:
private:
};