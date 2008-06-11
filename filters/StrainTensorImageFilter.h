# pragma once

#include "itkImage.h"
#include "itkImageToImageFilter.h"
#include "itkVector.h"

/**
 * \class StrainTensorImageFilter
 * \brief Computes the strain tensor, e, from a displacement field u = [u_x u_y]'.
 * The strain tensor describes the change in length of a small element undergoing deformation.
 * The strain tensor is defined as:
 * e_ij = 1/2 (du_i/dx_j + du_j/dx_i + du_l/dx_i * du_l/dx_j)
 * where i and j indicate spatial indices, and l indicates that an index applies to all spatial
 * indices as a sum.  So, in 2D, this becomes:
 * e_xx = 1/2 ( 2du_x/dx + (du_x/dx)^2 + (du_y/dx)^2 )
 * e_yy = 1/2 ( 2du_y/dy + (du_x/dy)^2 + (du_y/dy)^2 )
 * e_xy = 1/2 ( du_x/dy + du_y/dx + du_x/dx du_x/dy + du_y/dx du_y/dy )
 */
template < class TInputImage, class TOutputComponent = typename TInputImage::PixelType::ComponentType >
class StrainTensorImageFilter : 
    public itk::ImageToImageFilter< TInputImage, itk::Image< itk::Vector< TOutputComponent, 4 >, itk::GetImageDimension<TInputImage>::ImageDimension > >
{
public:
    // Input/output typedefs
    typedef TInputImage InputImageType;
    typedef TOutputComponent OutputComponentType;
    typedef itk::Vector< OutputComponentType, 4 > TensorPixelType;
    typedef itk::Image< TensorPixelType, InputImageType::ImageDimension > OutputImageType;

    // Common ITK typedefs
    typedef StrainTensorImageFilter Self;
    typedef itk::ImageToImageFilter< InputImageType, OutputImageType > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;

    itkNewMacro(Self);
    itkTypeMacro(GaussianGradientImageFilter, itk::ImageToImageFilter);
    itkStaticConstMacro(ImageDimension, unsigned int, InputImageType::ImageDimension);

    itkGetMacro(Sigma, double);
    itkSetMacro(Sigma, double);

protected:
    StrainTensorImageFilter() {};
    virtual ~StrainTensorImageFilter() {};
    
    void GenerateData();

private:
    // Not implemented
    StrainTensorImageFilter(const Self& other);
    void operator=(const Self& other);

    double m_Sigma;
};

//-------------------------------------
// Implementation
//-------------------------------------

#include "itkAdaptImageFilter.h"
#include "itkImageDuplicator.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkJoinImageFilter.h"
#include "itkNthElementPixelAccessor.h"
#include "itkRecursiveGaussianImageFilter.h"

#include "ImageUtils.h"
#include "Logger.h"

template < class TInputImage, class TOutputComponent >
void StrainTensorImageFilter< TInputImage, TOutputComponent >::GenerateData()
{
    std::string function("StrainTensorImageFilter::GenerateData");
    Logger::debug << function << ": entering..." << std::endl;
    
    // Allocate the output buffer
    Logger::debug << function << ": allocating output buffer" << std::endl;
    Superclass::AllocateOutputs();
    typename OutputImageType::Pointer output = this->GetOutput();
    typename OutputImageType::RegionType outRegion = output->GetRequestedRegion();

    // Compute the spatial derivatives of the input displacement field
    typedef itk::Image<OutputComponentType, ImageDimension> InternalImageType;
    typedef itk::AdaptImageFilter<InputImageType, InternalImageType, itk::NthElementPixelAccessor<OutputComponentType, typename InputImageType::PixelType > > SplitType;
    typedef itk::RecursiveGaussianImageFilter<InternalImageType, InternalImageType> GaussianType;
    typedef itk::ImageDuplicator< InternalImageType > CopyType;
    typedef itk::ImageRegionConstIterator< InternalImageType > ImageIterator;
    typedef itk::ImageRegionIterator< OutputImageType > TensorIterator;
    
    // We need to split the input into its component parts, and then take a derivative of each component in
    // each direction
    Logger::debug << function << ": computing displacement derivatives." << std::endl;
    typename SplitType::Pointer splitters[ImageDimension];
    typename GaussianType::Pointer gaussians[ImageDimension];
    typename CopyType::Pointer copy = CopyType::New();
    typename InternalImageType::Pointer images[ImageDimension][ImageDimension];
    ImageIterator dIts[ImageDimension][ImageDimension];

    for (int i = 0; i < ImageDimension; i++)
    {
        splitters[i] = SplitType::New();
        gaussians[i] = GaussianType::New();
    }

    int iDir = 0, dDir = 0;
    for (iDir = 0; iDir < ImageDimension; iDir++) // For each flow image dimension
    {
        // extract iDir'th component of displacement
        splitters[iDir]->SetInput(this->GetInput());
        splitters[iDir]->GetAccessor().SetElementNumber(iDir);

        for (dDir = 0; dDir < ImageDimension; dDir++) // For each derivative direction
        {
            // find derivative along dimension dDir
            for (int gDir = 0; gDir < ImageDimension; gDir++) // For each gaussian direction
            {
                gaussians[gDir]->SetSigma(this->GetSigma());
                gaussians[gDir]->SetDirection(gDir);
    
                if (dDir == gDir)   // take derivative along this dimension
                    gaussians[gDir]->SetFirstOrder();
                else                // just smooth along this dimension
                    gaussians[gDir]->SetZeroOrder();
                
                if (gDir == 0)      // input is from flow field
                    gaussians[gDir]->SetInput(splitters[iDir]->GetOutput());
                else                // input is from previous gaussian filter
                    gaussians[gDir]->SetInput(gaussians[gDir-1]->GetOutput());
                
                gaussians[gDir]->Update();
            }
            
            // Make a copy of the final image for this derivative direction
            copy->SetInputImage(gaussians[ImageDimension-1]->GetOutput());
            copy->Update();
            
            // Maintain a copy of the image data, and create an image iterator
            images[iDir][dDir] = copy->GetOutput();
            dIts[iDir][dDir] = ImageIterator(images[iDir][dDir], outRegion);
            dIts[iDir][dDir].GoToBegin();
            
            // save derivative image for debugging
//             char filename[80];
//             sprintf(filename, "deriv-%d%d.mha", iDir, dDir);
//             WriteImage<InternalImageType>(images[iDir][dDir], std::string(filename));
        } // done with this derivative direction
    } // done with this flow dimension

    // Compute the strain tensor at each point.  For indices we use the strain tensor equation from
    // L&L's Fundamental Equations chapter, page 2.  Except, we replace k with j, because it's unclear
    // why they feel a need to skip index letters.
    Logger::debug << function << ": computing strain tensor components" << std::endl;
    TensorIterator outTensor(output, outRegion);
    TensorPixelType pixel;
    bool done = false;
    for (outTensor.GoToBegin(); !(outTensor.IsAtEnd() || done) ; ++outTensor)
    {
        for (int i = 0; i < ImageDimension; i++)
        {
            for (int j = 0; j < ImageDimension; j++)
            {
                OutputComponentType sum = 0;
                for (int l = 0; l < ImageDimension; l++)
                    sum += dIts[l][i].Get() * dIts[l][j].Get();
                
                pixel[j + i*ImageDimension] = 0.5 * (dIts[i][j].Get() + dIts[j][i].Get() + sum);
            }
        }
        outTensor.Set(pixel);

        for (int i = 0; i < ImageDimension; i++)
            for (int j = 0; j < ImageDimension; j++)
            {
                ++dIts[i][j];
                done = done || dIts[i][j].IsAtEnd();
            }
    }
    
    Logger::verbose << function << ": grafting output" << std::endl;
    this->GraftOutput(output);
}
