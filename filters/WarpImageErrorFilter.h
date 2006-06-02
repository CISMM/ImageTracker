#pragma once

#import "itkImage.h"
#import "itkImageToImageFilter.h"
#import "itkSqrtImageFilter.h" 
#import "itkSquaredDifferenceImageFilter.h"
#import "itkWarpImageFilter.h"

/**
 * WarpImageErrorFilter computes the error in a deformation field used to map one image onto another.
 * That is, given two images and an deformation field, one image is warped using the deformation field
 * and compared to the second image. If the deformation field is an estimate of optical flow, the resultant
 * error image is an estimate of the error in the optical flow computation. This represents how much of
 * the change between the two images cannot be explained by the given deformation field.
 */
template < class TInputImage1, class TInputImage2, class TOutputImage, class TDeformationField >
class WarpImageErrorFilter :
    public itk::ImageToImageFilter< TInputImage1, TOutputImage >
{
public:
    // Standard ITK typedefs
    typedef WarpImageErrorFilter Self;
    typedef itk::ImageToImageFilter< TInputImage1, TOutputImage > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;

    itkNewMacro(Self);
    itkTypeMacro(WarpImageErrorFilter, ImageToImageFilter);

    // Useful typedefs
    typedef TInputImage1 InputType1;
    typedef TInputImage2 InputType2;
    typedef TOutputImage OutputType;
    typedef TDeformationField DeformationType;

    typedef itk::WarpImageFilter< InputType1, InputType1, DeformationType > WarpType;
    typedef itk::SquaredDifferenceImageFilter< InputType1, InputType2, OutputType > DifferenceType;
    typedef itk::SqrtImageFilter< OutputType, OutputType > SqrtType;

    // Image getters/setters

    TInputImage1* GetInput1()
    {
        return const_cast<TInputImage1*> (this->GetInput(0));
    }

    /**
     * Set the first input image. This image is warped with the deformation field.
     */
    void SetInput1(const TInputImage1* image1)
    {
        this->SetNthInput(0, const_cast<TInputImage1*> (image1));
    }

    TInputImage2* GetInput1()
    {
        return const_cast<TInputImage2*> (this->GetInput(1));
    }

    /**
     * Set the second input image. This image is compared to the warped first image.
     */
    void SetInput2(const TInputImage2* image2)
    {
        this->SetNthInput(1, const_cast<TInputImage2*> (image2));
    }

    TDeformationField* GetDeformationField()
    {
        return const_cast<TDeformationField*> (this->GetInput(2));
    }

    /**
     * Set the deformation field used to warp the first input image.
     */
    void SetDeformationField(const TDeformationField* field)
    {
        this->SetNthInput(2, const_cast<TDeformationField*> (field));
    }

protected:
    WarpImageErrorFilter() {}
    virtual ~WarpImageErrorFilter() {}

    void GenerateData();

private:
    WarpImageErrorFilter(const Self&);  // Not implemented
    operator=(const Self&);             // Not implemented
};

/** Implementation **/

template <class TInputImage1, class TInputImage2, class TOutputImage, class TDeformationField>
void WarpImageErrorFilter<TInputImage1, TInputImage2, TOutputImage, TDeformationField>
::GenerateData()
{
    // Allocate output
    Superclass::AllocateOutputs();

    // Instantiate objects
    WarpType::Pointer warper = WarpType::New();
    DifferenceType::Pointer diff = DifferenceType::New();
    SqrtType::Pointer sqrt = SqrtType::New();

    // Create pipeline
    warper->SetInput(this->GetInput1());
    warper->SetDeformationField(this->GetDeformationField());
    diff->SetInput1(this->warper->GetOutput());
    diff->SetInput2(this->GetInput2());
    sqrt->SetInput(diff->GetOutput());

    // Update and graft output
    sqrt->Update();
    this->GraftOutput(sqrt->GetOutput());
}