#include "MultiRegionRegistration.h"
#include "itkExceptionObject.h"

MultiRegionRegistration::MultiRegionRegistration(void)
{
    // Set up some default parameters
    this->roiRatio = 2.0;
    this->radiusOfInterest = 5;
}

MultiRegionRegistration::~MultiRegionRegistration(void)
{
}

void MultiRegionRegistration::SetFixedImage(InputImageType::Pointer fixed)
{
    this->fixedImage = fixed;
}

void MultiRegionRegistration::SetMovingImage(InputImageType::Pointer moving)
{
    this->movingImage = moving;
}

void MultiRegionRegistration::SetRadiusOfInterest(int pixels)
{
    this->radiusOfInterest = pixels;
}

void MultiRegionRegistration::SetROIRatio(double ratio)
{
    this->roiRatio = ratio;
}

MultiRegionRegistration::OutputImageType::Pointer MultiRegionRegistration::Update()
{
    if (this->movingImage == NULL || this->fixedImage == NULL)
    {
        throw exception("Fixed or moving image not setup.");
    }

    OutputImageType::Pointer output = this->CreateVectorImage(this->movingImage);

    this->InitProgress(this->movingImage);
    IteratorType moveIt(this->movingImage, this->movingImage->GetLargestPossibleRegion());
    IteratorType fixIt(this->fixedImage, this->fixedImage->GetLargestPossibleRegion());

    // Set up registration components
    //ImageRegistration::TransformType::Pointer pTransform = ImageRegistration::TransformType::New();
    //ImageRegistration::TransformType::ParametersType params(pTransform->GetNumberOfParameters());
    //ImageRegistration* pRegistration = new ImageRegistration();
    TransformType::Pointer pTransform = TransformType::New();
    TransformType::ParametersType params(pTransform->GetNumberOfParameters());
    
    pTransform = NULL;

    int fixRad = this->radiusOfInterest * this->roiRatio;
    int movRad = this->radiusOfInterest;

    // Run registration on each sub-image
    for (moveIt.GoToBegin(), fixIt.GoToBegin(); 
        !moveIt.IsAtEnd(); 
        ++moveIt, ++fixIt)
    {
        try
        {
            // We want this try/catch block because there is a possibility that a few
            // region registrations may fail (e.g. "All the points mapped outside of the
            // moving image") but we still want to continue with the other regions of
            // the image.
            
            //pTransform = pRegistration->Register(
            pTransform = this->DoRegister(
                this->CreateROI(fixedImage, fixIt.GetIndex(), fixRad),
                this->CreateROI(movingImage, moveIt.GetIndex(), movRad));
        }
        catch (itk::ExceptionObject &err)
        {
            std::cout << "Exception caught!" << std::endl << err << std::endl;
            pTransform = NULL;
        }

        if (pTransform == NULL)
        {
            // If there is a problem with registration, set the transform
            // params to zero.
            std::cout << "Warning!  Null transform resulted during MultiRegionRegistration." << std::endl;

            for (int i = 0; i < params.size(); i++)
            {
                params[i] = 0.0;
            }
        }
        else
        {
            params = pTransform->GetParameters();
        }

        // Save the transform in the vector image
        OutputImageType::PixelType outPixel;
        //outPixel[0] = (double) params[3];
        //outPixel[1] = (double) params[4];
        outPixel[0] = (double) params[0];
        outPixel[1] = (double) params[1];
        output->SetPixel(moveIt.GetIndex(), outPixel);

        if (moveIt.GetIndex()[0] == 0)
            this->UpdateProgress(moveIt.GetIndex(), outPixel);
    }

    //delete pRegistration;

    return output;
}

/**
 * Does registration for only translation transforms.
 */
MultiRegionRegistration::TransformType::Pointer MultiRegionRegistration::DoRegister(InputImageType::Pointer fixed, InputImageType::Pointer moving)
{
    // Set up registration objects
    MetricType::Pointer metric = MetricType::New();
    TransformType::Pointer transform = TransformType::New();
    OptimizerType::Pointer optimizer = OptimizerType::New();
    InterpolatorType::Pointer interpolator = InterpolatorType::New();
    RegistrationType::Pointer registration = RegistrationType::New();

    // Set up registration pipeline
    registration->SetMetric(metric);
    registration->SetOptimizer(optimizer);
    registration->SetTransform(transform);
    registration->SetInterpolator(interpolator);

    // Set up images
    registration->SetFixedImage(fixed);
    registration->SetMovingImage(moving);
    registration->SetFixedImageRegion(fixed->GetBufferedRegion());

    // Set intial transform
    TransformType::ParametersType init(transform->GetNumberOfParameters());
    init[0] = 0.0;
    init[1] = 0.0;
    registration->SetInitialTransformParameters(init);

    // Set up optimizer
    // optimizer->SetMaximumStepLength(this->radiusOfInterest);
    optimizer->SetMaximumStepLength(0.5);
    optimizer->SetMinimumStepLength(0.005);
    optimizer->SetNumberOfIterations(200);

    registration->StartRegistration();

    // Get results
    TransformType::Pointer finalTran = TransformType::New();
    TransformType::ParametersType params = registration->GetLastTransformParameters();
    finalTran->SetParameters(params);

    //std::cout << "Iterations: " << optimizer->GetCurrentIteration() << std::endl;

    return finalTran;
}

MultiRegionRegistration::InputImageType::Pointer MultiRegionRegistration::CreateROI(InputImageType::Pointer image, InputImageType::IndexType pixelCenter, int pixelRadius)
{
    InputImageType::RegionType largestRegion = image->GetLargestPossibleRegion();

    // @todo: Figure out how to size the region if it would fall outside
    // the image.
    // Setup the region of interest
    InputImageType::RegionType region;

    for (int i = 0; i < InputImageType::RegionType::ImageDimension; i++)
    {
        region.SetIndex(i, pixelCenter[i] - pixelRadius);
        region.SetSize(i, 2*pixelRadius + 1);

        if (region.GetIndex(i) < largestRegion.GetIndex(i))
        {
            region.SetIndex(i, largestRegion.GetIndex(i));
            region.SetSize(i, pixelCenter[i] + pixelRadius - region.GetIndex(i));
        }
        if (region.GetIndex(i) + region.GetSize(i) > largestRegion.GetSize(i))
        {
            region.SetSize(i, largestRegion.GetSize(i) - region.GetIndex(i));
        }
    }

    // Debug -- print regions
#ifdef DEBUG
    std::cout << "Center: " << pixelCenter << std::endl;
    std::cout << "Radius: " << pixelRadius << std::endl;
    std::cout << "Region: ";
    region.Print(std::cout);
    std::cout << std::endl;
#endif

    // Use a ROI filter to create an image of the proper size
    ROIFilterType::Pointer roiFilter = ROIFilterType::New();
    roiFilter->SetRegionOfInterest(region);
    roiFilter->SetInput(image);
    roiFilter->Update();
    return roiFilter->GetOutput();
}

MultiRegionRegistration::OutputImageType::Pointer MultiRegionRegistration::CreateVectorImage(const InputImageType::Pointer image)
{
    // Set up the output image with all the same dimensions,
    // spacings, and sizes as the input image.
    OutputImageType::Pointer output = OutputImageType::New();
    OutputImageType::RegionType region;
    InputImageType::RegionType inRegion = image->GetLargestPossibleRegion();
    region.SetSize(inRegion.GetSize());
    region.SetIndex(inRegion.GetIndex());
    output->SetRegions(region);
    output->Allocate();

    output->SetOrigin(image->GetOrigin());
    output->SetSpacing(image->GetSpacing());

    return output;
}

void MultiRegionRegistration::InitProgress(InputImageType::Pointer image)
{
    this->size = image->GetLargestPossibleRegion().GetSize();
}

void MultiRegionRegistration::UpdateProgress(InputImageType::IndexType index, OutputImageType::PixelType pixel)
{
    double dIndex = (double) index[1];
    double percentDone = (double) dIndex / this->size[1] * 100.0;
    std::cout << percentDone << " %  (" << dIndex << " / " << this->size[1] << ")" << std::endl;

    // Print params:
    //std::cout << "Translation: " << std::endl;
    //std::cout << " x => " << pixel[0] << std::endl;
    //std::cout << " y => " << pixel[1] << std::endl;
    //std::cout << "------------\n\n\n";
}