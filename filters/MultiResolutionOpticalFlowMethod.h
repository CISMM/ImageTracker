#pragma once

#include "OpticalFlowImageFilter.h"

template < class TFixedImage, class TMovingImage >
class MultiResolutionOpticalFlowMethod : 
    public OpticalFlowImageFilter< TFixedImage, TMovingImage >
{
public:
    typedef TFixedImage FixedImageType;
    typedef typename FixedImageType::ConstPointer FixedImagePointer;
    typedef TMovingImage MovingImageType;
    typedef typename MovingImageType::ConstPointer MovingImagePointer;
    typedef FixedImageType ImageType;
    
    typedef MultiResolutionOpticalFlowMethod Self;
    typedef OpticalFlowImageFilter< TFixedImage, TMovingImage > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    itkTypeMacro(MultiResolutionOpticalFlowMethod, OpticalFlowImageFilter);
    
    typedef OpticalFlowImageFilter< FixedImageType, MovingImageType > OpticalFlowType;
    typedef typename OpticalFlowType::Pointer OpticalFlowPointer;
    typedef typename OpticalFlowType::OutputImageType OutputImageType;
    typedef typename OutputImageType::Pointer OutputImagePointer;
    typedef typename OutputImageType::PixelType OutputPixelType;
    
    /** Get/Set the number of levels in the multiresolution flow computation. */
    itkGetMacro(NumberOfLevels, unsigned int);
    itkSetMacro(NumberOfLevels, unsigned int);
    
    /** Get/Set the optical flow computation method. */
    itkGetObjectMacro(OpticalFlow, OpticalFlowType);
    itkSetObjectMacro(OpticalFlow, OpticalFlowType);
    
    /** Get/Set the fixed input image. */
//     itkGetConstObjectMacro(FixedImage, FixedImageType);
//     itkSetConstObjectMacro(FixedImage, FixedImageType);
    
    /** Get/Set the moving input image. */
//     itkGetConstObjectMacro(MovingImage, MovingImageType);
//     itkSetConstObjectMacro(MovingImage, MovingImageType);
    
protected:
    MultiResolutionOpticalFlowMethod() {}
    ~MultiResolutionOpticalFlowMethod() {}
    
    void GenerateData();
    
private:
    MultiResolutionOpticalFlowMethod(const Self& other);
    void operator=(const Self& other);
    
    unsigned int m_NumberOfLevels;
    OpticalFlowPointer m_OpticalFlow;
//     FixedImagePointer m_FixedImage;
//     MovingImagePointer m_MovingImage;
};

//------- Implementation --------//

#include "itkAddImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkMultiResolutionPyramidImageFilter.h"
#include "itkSimilarity2DTransform.h"
#include "itkUnaryFunctorImageFilter.h"
#include "itkVectorResampleImageFilter.h"
#include "itkVectorRescaleIntensityImageFilter.h"
#include "itkWarpImageFilter.h"

#include "Logger.h"
#include "ImageUtils.h"

template < class TFixedImage, class TMovingImage >
void MultiResolutionOpticalFlowMethod< TFixedImage, TMovingImage >::GenerateData()
{
    std::string function("MultiResolutionOpticalFlowMethod::GenerateData");
    
    
    typedef itk::MultiResolutionPyramidImageFilter< FixedImageType, ImageType > FixedPyramidType;
    typedef itk::MultiResolutionPyramidImageFilter< MovingImageType, ImageType > MovingPyramidType;
    typedef itk::WarpImageFilter< ImageType, ImageType, OutputImageType > WarpType;
    typedef itk::AddImageFilter< OutputImageType, OutputImageType, OutputImageType > AddType;
    typedef itk::VectorResampleImageFilter< OutputImageType, OutputImageType > ResampleType;
    typedef itk::UnaryFunctorImageFilter< OutputImageType, OutputImageType, itk::Functor::VectorMagnitudeLinearTransform< OutputPixelType, OutputPixelType > > ScaleType;
    
    // Allocate output
    Superclass::AllocateOutputs();
    
    Logger::debug << function << ": Setting up image pyramids" << std::endl;
    // A multi-resolution image pyramid for the fixed input image
    typename FixedPyramidType::Pointer fixedPyr = FixedPyramidType::New();
    fixedPyr->SetInput(this->GetInput1());
    fixedPyr->SetNumberOfLevels(this->GetNumberOfLevels());
    fixedPyr->Update();
    
    // A multi-resolution image pyramid for the moving input image
    typename MovingPyramidType::Pointer movingPyr = MovingPyramidType::New();
    movingPyr->SetInput(this->GetInput2());
    movingPyr->SetNumberOfLevels(this->GetNumberOfLevels());
    movingPyr->Update();
    
    Logger::debug << function << ": Creating initial flow field" << std::endl;
    // Create an initially empty flow field
    OutputImagePointer currentFlow = OutputImageType::New();
    currentFlow->SetRegions(movingPyr->GetOutput(0)->GetLargestPossibleRegion());
    currentFlow->Allocate();
    
    typename OutputImageType::PixelType zero;
    zero.Fill(0.0);

    typedef itk::ImageRegionIterator< OutputImageType > IteratorType;
    IteratorType iter(currentFlow, currentFlow->GetLargestPossibleRegion());
    for (iter.GoToBegin(); !iter.IsAtEnd(); ++iter)
    {
        iter.Set(zero);
    }
    
    Logger::debug << function << ": Creating flow adder, resampler, and rescaler" << std::endl;
    // Warp the moving image with the current optical flow estimate
    typename WarpType::Pointer warp = WarpType::New();
    
    // Compute updated optical flow using in loop below
    
    // Add the differential flow update to the current flow estimate
    typename AddType::Pointer add = AddType::New();
    
    // Upsample the current flow estimate for the next level
    typename ResampleType::Pointer resample = ResampleType::New();
    resample->SetInput(add->GetOutput());
    typedef itk::Similarity2DTransform< double > TransformType;
    typename TransformType::Pointer transform = TransformType::New();
    transform->SetScale(2.0);
    resample->SetTransform(transform);
    
    // Rescale the flow estimate due to upsampling
    typename ScaleType::Pointer scale = ScaleType::New();
    scale->SetInput(resample->GetOutput());
    scale->GetFunctor().SetFactor(2.0);
    
    Logger::debug << function << ": Compute multiple resolution optical flow..." << std::endl;
    // Perform optical flow computation at each level of the pyramid
    for (int level = 0; level < this->GetNumberOfLevels(); level++)
    {
        int plevel = this->GetNumberOfLevels() - level;
        PrintImageInfo(fixedPyr->GetOutput(level), "Fixed image", Logger::debug);
        PrintImageInfo(movingPyr->GetOutput(level), "Moving image", Logger::debug);
        PrintImageInfo< OutputImageType >(currentFlow, "Current flow", Logger::debug);
//         typename InputImageType::RegionType region = fixedPyr->GetOutput(level)->GetLargestPossibleRegion();
        
        Logger::debug << function << ": Level => " << plevel << " setting up moving image warp" << std::endl;
        // Warp the moving image with the current optical flow estimate
        warp->SetInput(movingPyr->GetOutput(level));
        warp->SetDeformationField(currentFlow);
        
        Logger::debug << function << ": Level => " << plevel << " setting up flow computation" << std::endl;
        // Compute optical flow at current resolution level using the optical flow method
        // provided by the caller
        this->GetOpticalFlow()->SetInput1(fixedPyr->GetOutput(level));
        this->GetOpticalFlow()->SetInput2(warp->GetOutput());
        
        Logger::debug << function << ": Level => " << plevel << " setting up flow addition" << std::endl;
        // Add the differential flow update to the current flow estimate
        add->SetInput1(currentFlow);
        add->SetInput2(this->GetOpticalFlow()->GetOutput());
        
        Logger::debug << function << ": Level => " << plevel << " executing warp, flow, add" << std::endl;
        add->Update();
        
        if (level == 1) break; // Done...no need to resample and rescale
        
        Logger::debug << function << ": Level => " << plevel << " executing resample, rescale" << std::endl;
        scale->Update();
        currentFlow = scale->GetOutput();
        currentFlow->DisconnectPipeline();
    }
    
    this->GraftOutput(currentFlow);
}
