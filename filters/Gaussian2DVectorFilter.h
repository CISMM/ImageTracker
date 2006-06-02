#pragma once

#include "itkFixedArray.h"
#include "itkImage.h"
#include "itkImageToImageFilter.h"
#include "itkVector.h"

template < class TInputImage, class TOutputImage >
class Gaussian2DVectorFilter :
    public itk::ImageToImageFilter< TInputImage, TOutputImage >
{
public:
    /** Standard itk class typedefs. */
    typedef Gaussian2DVectorFilter Self;
    typedef itk::ImageToImageFilter< TInputImage, TOutputImage > Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    /** Method for creation through object factory */
    itkNewMacro(Self);

    /** Run-time type information */
    itkTypeMacro(Gaussian2DVectorFilter, ImageToImageFilter);

    /** Image type information */
    typedef TInputImage InputImageType;
    typedef TOutputImage OutputImageType;

    typedef typename Superclass::OutputImageRegionType OutputImageRegionType;

    /** Extract info from image types. */
    itkStaticConstMacro(ImageDimension, 
        unsigned int, 
        OutputImageType::ImageDimension);

    typedef itk::FixedArray< double, itkGetStaticConstMacro(ImageDimension) > ArrayType;

    /** Standard get/set macros for filter parameters */
    itkSetMacro(Variance, ArrayType);
    itkGetMacro(Variance, const ArrayType);

    void SetVariance(const double v)
    {
        m_Variance.Fill(v);
    }

protected:
    Gaussian2DVectorFilter()
    {
        this->m_Variance.Fill(1.0);
        this->m_Components = 2;
    }

    virtual ~Gaussian2DVectorFilter() {}

    void GenerateData();
    void PrintSelf(std::ostream& os, itk::Indent indent) const;

private:
    Gaussian2DVectorFilter(const Self&); // not implemented
    void operator=(const Self&); // not implemented

    ArrayType m_Variance;

    /** Number of components per vector (2) */
    unsigned int m_Components;
};

// Implementation

#include "Gaussian2DVectorFilter.h"
#include "itkCompose2DVectorImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkVectorIndexSelectionCastImageFilter.h"

template< class TInputImage, class TOutputImage >
void Gaussian2DVectorFilter< TInputImage, TOutputImage >
::GenerateData()
{
    // Initialize output
    typename TOutputImage::Pointer output = this->GetOutput();
    output->SetBufferedRegion(output->GetRequestedRegion());
    output->Allocate();

    // Typedef of filters to split vector image, smooth, and recompose image
    typedef InputImageType::PixelType::ComponentType ScalarType;
    typedef itk::Image<ScalarType, itkGetStaticConstMacro(ImageDimension) > ScalarImageType;
    typedef itk::VectorIndexSelectionCastImageFilter<InputImageType, ScalarImageType> DecomposeType;
    typedef itk::DiscreteGaussianImageFilter<ScalarImageType, ScalarImageType> FilterType;
    typedef itk::Compose2DVectorImageFilter<ScalarImageType, OutputImageType> ComposeType;

    // Create filter components
    DecomposeType::Pointer *decomp = new DecomposeType::Pointer[this->m_Components];
    FilterType::Pointer *smooth = new FilterType::Pointer[this->m_Components];
    ComposeType::Pointer comp = ComposeType::New();

    // Initialize and connect filter components
    for (unsigned int i = 0; i < this->m_Components; i++)
    {
        // set up decomposition and smoothing
        decomp[i] = DecomposeType::New();
        decomp[i]->SetIndex(i);
        smooth[i] = FilterType::New();
        smooth[i]->SetVariance(this->m_Variance);

        // Connect filter components
        decomp[i]->SetInput(this->GetInput());
        smooth[i]->SetInput(decomp[i]->GetOutput());
        comp->SetInput(i, smooth[i]->GetOutput());
    }

    // Graft output to write results to the bulk data output
    comp->GraftOutput(output);
    // Update the last filter component
    comp->Update();
    // Graft output onto this filter's output
    this->GraftOutput(output);

    // Clean up filter sets
    delete[] decomp;
    delete[] smooth;
}

template< class TInputImage, class TOutputImage >
void Gaussian2DVectorFilter< TInputImage, TOutputImage >
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
    Superclass::PrintSelf(os, indent);

    os << indent << "Variance: " << this->m_Variance << std::endl;
    os << indent << "Components: " << this->m_Components << std::endl;
}