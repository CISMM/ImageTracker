#pragma once

#include "OpticalFlowImageFilter.h"
#include "CommonTypes.h"

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
#include "itkRecursiveMultiResolutionPyramidImageFilter.h"
#include "itkSimilarity2DTransform.h"
#include "itkStatisticsImageFilter.h"
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
    
    typedef itk::RecursiveMultiResolutionPyramidImageFilter< FixedImageType, ImageType > FixedPyramidType;
    typedef itk::RecursiveMultiResolutionPyramidImageFilter< MovingImageType, ImageType > MovingPyramidType;
    typedef itk::WarpImageFilter< ImageType, ImageType, OutputImageType > WarpType;
    typedef itk::StatisticsImageFilter< ImageType > StatsType;
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
    
    Logger::debug << function << ": Fixed pyramid has  " << fixedPyr->GetNumberOfOutputs() << " output images." << std::endl;
    Logger::debug << function << ": Moving pyramid has " << movingPyr->GetNumberOfOutputs() << " output images." << std::endl;
    
    Logger::debug << function << ": Creating initial flow field" << std::endl;
    // Create an initially empty flow field
    OutputImagePointer currentFlow = OutputImageType::New();
    MovingImagePointer lowresImg = movingPyr->GetOutput(0);
    currentFlow->SetRegions(lowresImg->GetLargestPossibleRegion());
    currentFlow->SetOrigin(lowresImg->GetOrigin());
    currentFlow->SetSpacing(lowresImg->GetSpacing());
    currentFlow->Allocate();
    
    // Fill initial flow field with [0,0] vectors
    typename OutputImageType::PixelType zero;
    zero.Fill(0.0);
    typedef itk::ImageRegionIterator< OutputImageType > IteratorType;
    IteratorType iter(currentFlow, currentFlow->GetLargestPossibleRegion());
    for (iter.GoToBegin(); !iter.IsAtEnd(); ++iter)
    {
        iter.Set(zero);
    }
    
    Logger::debug << function << ": Creating flow warper, adder, resampler, and rescaler" << std::endl;
    // Warp the moving image with the current optical flow estimate
    typename WarpType::Pointer warp = WarpType::New();
    typename StatsType::Pointer stats = StatsType::New(); // used to compute warping outside value
    
    // Compute updated optical flow using in loop below
    
    // Add the differential flow update to the current flow estimate
    typename AddType::Pointer add = AddType::New();
    
    // Upsample the current flow estimate for the next level
    typename ResampleType::Pointer resample = ResampleType::New();
    typedef itk::Similarity2DTransform< double > TransformType;
    typename TransformType::Pointer transform = TransformType::New();
    // Somehow, I guess because the spacing halves and region size doubles at each
    // step of the multi-res pyramid, the scaling we use in the transform is just 1.
    transform->SetScale(1.0);
    resample->SetTransform(transform);
    
    // Rescale the flow estimate due to upsampling
    typename ScaleType::Pointer scale = ScaleType::New();
    scale->GetFunctor().SetFactor(2.0);
    scale->SetInput(resample->GetOutput());
    scale->InPlaceOff();
    
    Logger::debug << function << ": Compute multiple resolution optical flow..." << std::endl;
    // Perform optical flow computation at each level of the pyramid
    for (int level = 0; level < this->GetNumberOfLevels(); level++)
    {
        FixedImagePointer fixedImg = fixedPyr->GetOutput(level);
        MovingImagePointer movingImg = movingPyr->GetOutput(level);
                
        Logger::debug << function << ": Level => " << level << " warping moving image with current flow estimate" << std::endl;
        
        stats->SetInput(movingImg);
        stats->Update();
        // Warp the moving image with the current optical flow estimate
        warp->SetInput(movingImg);
        warp->SetDeformationField(currentFlow);
        warp->SetEdgePaddingValue(stats->GetMean());
        warp->SetOutputOrigin(movingImg->GetOrigin());
        warp->SetOutputSpacing(movingImg->GetSpacing());
        warp->UpdateLargestPossibleRegion();
        
        Logger::debug << function << ": Level => " << level << " Flow input image information:" << std::endl;
        PrintImageInfo<FixedImageType>(fixedImg, "Fixed image", Logger::debug);
        PrintImageInfo<MovingImageType>(movingImg, "Moving image", Logger::debug);
        PrintImageInfo(warp->GetOutput(), "Warped image", Logger::debug);
        PrintImageInfo<OutputImageType>(currentFlow, "Current flow", Logger::debug);
        
//         char filename[80];
//         sprintf(filename, "warp-lev-%d.tif", level);
//         WriteImage< MovingImageType, CommonTypes::InputImageType >(warp->GetOutput(), std::string(filename), false);
        
        Logger::debug << function << ": Level => " << level << " setting up flow computation" << std::endl;
        // Compute optical flow at current resolution level using the optical flow method
        // provided by the caller
        this->GetOpticalFlow()->SetInput1(fixedImg);
        this->GetOpticalFlow()->SetInput2(warp->GetOutput());
        this->GetOpticalFlow()->SetInitialFlow(currentFlow);
        this->GetOpticalFlow()->UpdateLargestPossibleRegion();
        
        Logger::debug << function << ": Level => " << level << " setting up flow addition" << std::endl;
        // Add the differential flow update to the current flow estimate
        add->SetInput1(currentFlow);
        add->SetInput2(this->GetOpticalFlow()->GetOutput());
        
        Logger::debug << function << ": Level => " << level << " executing flow, add" << std::endl;
        add->UpdateLargestPossibleRegion();
        
//         sprintf(filename, "add-lev-%d.mha", level);
//         WriteImage<OutputImageType>(add->GetOutput(), std::string(filename));
        
        if (level == this->GetNumberOfLevels()-1) break; // Done...no need to resample and rescale
        
        Logger::debug << function << ": Level => " << level << " resampling flow for next level" << std::endl;
        // Setup region information for resampling
        resample->SetInput(add->GetOutput());
        FixedImagePointer nextImg = fixedPyr->GetOutput(level+1);
        resample->SetOutputStartIndex(nextImg->GetLargestPossibleRegion().GetIndex());
        resample->SetSize(nextImg->GetLargestPossibleRegion().GetSize());
        resample->SetOutputOrigin(nextImg->GetOrigin());
        resample->SetOutputSpacing(nextImg->GetSpacing());
        resample->UpdateLargestPossibleRegion();
        
//         sprintf(filename, "resample-lev-%d.mha", level);
//         WriteImage<OutputImageType>(resample->GetOutput(), std::string(filename));
        
        Logger::debug << function << ": Level => " << level << " rescaling flow for next level" << std::endl;
        scale->UpdateLargestPossibleRegion();
        
//         sprintf(filename, "rescale-lev-%d.mha", level);
//         WriteImage<OutputImageType>(scale->GetOutput(), std::string(filename));
        
        currentFlow = scale->GetOutput();
        currentFlow->DisconnectPipeline();
        
        Logger::debug << function << ": Level => " << level << " Flow output image information:" << std::endl;
        PrintImageInfo< OutputImageType >(this->GetOpticalFlow()->GetOutput(), "Flow output", Logger::debug);
        PrintImageInfo< OutputImageType >(add->GetOutput(), "Add output", Logger::debug);
        PrintImageInfo< OutputImageType >(resample->GetOutput(), "Resample output", Logger::debug);
        PrintImageInfo< OutputImageType >(scale->GetOutput(), "Scale output", Logger::debug);
        PrintImageInfo< OutputImageType >(currentFlow, "Updated flow", Logger::debug);
    }
    
    Logger::debug << function << "Final flow output image information:" << std::endl;
    PrintImageInfo< OutputImageType >(this->GetOpticalFlow()->GetOutput(), "Flow output", Logger::debug);
    PrintImageInfo< OutputImageType >(add->GetOutput(), "Add output", Logger::debug);

    this->GraftOutput(add->GetOutput());
    PrintImageInfo< OutputImageType >(this->GetOutput(), "MR flow output", Logger::debug);
}
