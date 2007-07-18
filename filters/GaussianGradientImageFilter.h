#pragma once

#include "itkImage.h"
#include "itkImageToImageFilter.h"
#include "itkVector.h"

/**
 * \class GaussianGradientImageFilter
 * \brief Computes the gradient of an input image using a set of Gaussian and derivative of Gaussian image filters.
 *
 * The gradient is computed at the observation scale of the standard deviation of the Gaussian filters.  The Gradient
 * consists of a partial derivative along each of the input image dimensions.  Each partial derivative is computed by
 * smoothing the image along the other directions and convolving with a derivative of Gaussian along the derivative
 * direction.
 */
template < class TInputImage, class TOutputPixel = typename TInputImage::PixelType >
class GaussianGradientImageFilter
    : public itk::ImageToImageFilter< TInputImage, itk::Image< itk::Vector< TOutputPixel, itk::GetImageDimension<TInputImage>::ImageDimension >, itk::GetImageDimension<TInputImage>::ImageDimension > >
{
public:  
    // Useful typedefs
    typedef TInputImage InputImageType;
    typedef typename InputImageType::PixelType InputPixelType;
    typedef TOutputPixel OutputPixelType;
    typedef itk::Image < OutputPixelType, InputImageType::ImageDimension > PartialImageType;
    typedef itk::Vector< OutputPixelType, InputImageType::ImageDimension > GradientPixelType;
    typedef itk::Image< GradientPixelType, InputImageType::ImageDimension > OutputImageType;
    
    // Standard itk typedefs
    typedef GaussianGradientImageFilter Self;
    typedef itk::ImageToImageFilter< TInputImage, OutputImageType > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;

    itkNewMacro(Self);
    itkTypeMacro(GaussianGradientImageFilter, itk::ImageToImageFilter);
    itkStaticConstMacro(ImageDimension, unsigned int, InputImageType::ImageDimension);
    
    itkGetMacro(Sigma, float);
    itkSetMacro(Sigma, float);
    
protected:
    GaussianGradientImageFilter(){}
    virtual ~GaussianGradientImageFilter(){}
    
    void GenerateData();
    
private:
    // Not implemented
    GaussianGradientImageFilter(const Self& other);
    void operator=(const Self& other);
    
    float m_Sigma;
};

//----------------- Implementation -----------------//

#include "itkRecursiveGaussianImageFilter.h"
#include "itkJoinImageFilter.h"

template < class TInputImage, class TOutputPixel >
void GaussianGradientImageFilter< TInputImage, TOutputPixel >::GenerateData()
{
    // Allocate output
    Superclass::AllocateOutputs();
    
    typedef itk::RecursiveGaussianImageFilter< InputImageType, PartialImageType > GaussianType;
    typedef itk::JoinImageFilter< PartialImageType, PartialImageType > JoinType;
    
    // Create a 2D array of Gaussian filters.  The first dimension of the array (dim) indicates the
    // dimension of the image for which the partial derivative is being computed.  The second
    // dimension of the array (dir) is the direction along which the Gaussian filter operates.  So,
    // where dim=dir, the derivative is computed along that direction, so the first order Gaussian is
    // used.  The last dir element for each dim array is the result of the smoothed partial derivative
    // for the dim direction.
    typename GaussianType::Pointer gaussians[ImageDimension][ImageDimension];
    typename JoinType::Pointer join = JoinType::New();
    for (int dim = 0; dim < ImageDimension; dim++)
    {
        for (int dir = 0; dir < ImageDimension; dir++)
        {
            gaussians[dim][dir] = GaussianType::New();
            gaussians[dim][dir]->SetSigma(this->GetSigma());
            gaussians[dim][dir]->SetDirection(dir);
            
            if (dim == dir)
                gaussians[dim][dir]->SetFirstOrder();
            else
                gaussians[dim][dir]->SetZeroOrder();
            
            if (dir == 0)
                gaussians[dim][dir]->SetInput(this->GetInput());
            else
                gaussians[dim][dir]->SetInput(gaussians[dim][dir-1]->GetOutput());
        }
        join->SetInput(dim, gaussians[dim][ImageDimension-1]->GetOutput());
    }
    
    join->Update();
    this->GraftOutput(join->GetOutput());
}
