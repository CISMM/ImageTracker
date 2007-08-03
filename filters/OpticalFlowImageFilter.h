#pragma once

#include "itkImage.h"
#include "itkImageToImageFilter.h"
#include "itkVector.h"

template <class TInputImage1, class TInputImage2, class TOutputValueType = float >
class OpticalFlowImageFilter :
        public itk::ImageToImageFilter< TInputImage1,
        itk::Image< itk::Vector <TOutputValueType, itk::GetImageDimension<TInputImage1>::ImageDimension>, ::itk::GetImageDimension<TInputImage1>::ImageDimension> >
{
public:
    /** Some convenient typedefs */
    typedef TInputImage1 Input1ImageType;
    typedef typename Input1ImageType::ConstPointer Input1ImagePointer;
    typedef typename Input1ImageType::RegionType Input1ImageRegionType;
    typedef typename Input1ImageType::PixelType Input1ImagePixelType;
    typedef TInputImage2 Input2ImageType;
    typedef typename Input2ImageType::ConstPointer Input2ImagePointer;
    typedef typename Input2ImageType::RegionType Input2ImageRegionType;
    typedef typename Input2ImageType::PixelType Input2ImagePixelType;
    
    /** Image typedef support */
    itkStaticConstMacro(ImageDimension, unsigned int, Input1ImageType::ImageDimension);
    typedef typename Input1ImageType::PixelType InputPixelType;
    typedef Input1ImageType InputImageType;

    typedef TOutputValueType OutputValueType;
    //typedef itk::Vector<OutputValueType, ImageDimension> InputPixelType;
    //typedef itk::Image<InputPixelType, ImageDimension> InputImageType;
    typedef itk::Vector<OutputValueType, ImageDimension> OutputPixelType;
    typedef itk::Image<OutputPixelType, ImageDimension> OutputImageType;
    typedef typename InputImageType::Pointer InputImagePointer;
    typedef typename OutputImageType::Pointer OutputImagePointer;
    typedef typename OutputImageType::RegionType OutputImageRegionType;
    typedef OutputPixelType VectorType;

    /** Standard itk class typedefs */
    typedef OpticalFlowImageFilter Self;
    typedef itk::ImageToImageFilter<InputImageType, OutputImageType> Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;
    
    itkTypeMacro(OpticalFlowImageFilter, ImageToImageFilter);
    
    TInputImage1 * GetInput1()
    {
        return const_cast<TInputImage1 *> (this->GetInput(0));
    }

    /** 
     * Set the first (moving) image for Optic Flow. Note, the image
     * data must be loaded (updated) before this filter's Update() 
     * method is called.
     */
    void SetInput1(const TInputImage1 * image1)
    {
        this->SetNthInput(0, const_cast<TInputImage1 *>(image1));
    }

    TInputImage2 * GetInput2()
    {
        return const_cast<TInputImage2 *> (this->GetInput(1));
    }

    /** 
     * Set the second (fixed) image for Optic Flow. Note, the image
     * data must be loaded (updated) before this filter's Update() 
     * method is called.
     */
    void SetInput2(const TInputImage2 * image2)
    {
        this->SetNthInput(1, const_cast<TInputImage2 *>(image2));
    }
    
protected:
    OpticalFlowImageFilter(){}
    ~OpticalFlowImageFilter(){}
    
private:
    OpticalFlowImageFilter(const Self& other);
    void operator=(const Self& other);
};
