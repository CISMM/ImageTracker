#include <iostream>

#include "ImageRegistration.h"

ImageRegistration::ImageRegistration(void)
{
    // Create a default blur schedule (no blurring)
    this->pBlurSchedule = new BlurFilterSchedule(0.0, 0.0, 1, 10);
}

ImageRegistration::~ImageRegistration(void)
{
}

ImageRegistration::TransformType::Pointer ImageRegistration::Register(ImageType::Pointer fixedImage, ImageType::Pointer movingImage)
{
    //Register using a default initial transform
    return this->Register(fixedImage, movingImage, this->DefaultInitialTransform(fixedImage, movingImage));
}

ImageRegistration::TransformType::Pointer ImageRegistration::Register(ImageType::Pointer fixedImage, ImageType::Pointer movingImage, TransformType::Pointer initialTransform)
{
    //Use the initial transform supplied
    TransformType::Pointer transform = TransformType::New();
    transform->SetParameters(initialTransform->GetParameters());

    //Create a blur schedule for each image
    // @todo Set up image blurring
    BlurFilterSchedule* fixedBlurSchedule = this->pBlurSchedule->Clone();
    BlurFilterSchedule* movingBlurSchedule = this->pBlurSchedule->Clone();
    fixedBlurSchedule->SetSourceImage(fixedImage);
    movingBlurSchedule->SetSourceImage(movingImage);

    //Register images at each blur level
    bool verbose = true;
    while (fixedBlurSchedule->HasMoreLevels() && movingBlurSchedule->HasMoreLevels())
    {
        transform = this->DoRegister(
            fixedBlurSchedule->NextBlurredImage(),
            movingBlurSchedule->NextBlurredImage(),
            transform, verbose);
        if (verbose)
        {
            std::cout << "Registered at blur level -> " << fixedBlurSchedule->GetCurrentLevel() << std::endl;
        }
    }

    delete fixedBlurSchedule;
    delete movingBlurSchedule;

    return transform;
}

void ImageRegistration::SetBlurSchedule(BlurFilterSchedule* pBlurSchedule)
{
    this->pBlurSchedule = pBlurSchedule;
}

ImageRegistration::TransformType::InputPointType ImageRegistration::FindImageCenter(const ImageType::Pointer image)
{
    const ImageType::SpacingType& spacing = image->GetSpacing();
    const ImageType::PointType& origin = image->GetOrigin();

    ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();

    TransformType::InputPointType center;
    center[0] = origin[0] + spacing[0] * size[0] / 2.0;
    center[1] = origin[1] + spacing[1] * size[1] / 2.0;

    return center;
}

void ImageRegistration::SetupOptimizer(OptimizerType::Pointer optimizer, TransformType::Pointer transform)
{
    //Set up scaling for the different units of the transformation, keeping in mind
    //that angles of rotation are typically small compared to the translation.
    OptimizerType::ScalesType optimizerScales(transform->GetNumberOfParameters());

    const double translationScale = 1.0 / 1000.0;

    optimizerScales[0] = 1.0;
    optimizerScales[1] = translationScale;
    optimizerScales[2] = translationScale;
    optimizerScales[3] = translationScale;
    optimizerScales[4] = translationScale;

    optimizer->SetScales(optimizerScales);

    //Set up step lengths
    optimizer->SetMaximumStepLength(0.5);
    optimizer->SetMinimumStepLength(0.01);
    optimizer->SetNumberOfIterations(200);

    //Set up an observer to record optimizer info at each optimization stage
    //CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
    //optimizer->AddObserver(itk::IterationEvent(), observer);
}

ImageRegistration::TransformType::Pointer ImageRegistration::DefaultInitialTransform(ImageType::Pointer fixedImage, ImageType::Pointer movingImage)
{
    fixedImage->Update();
    movingImage->Update();

    //Find the centers of the images        
    TransformType::InputPointType centerFixed =  this->FindImageCenter(fixedImage);
    TransformType::InputPointType centerMoving = this->FindImageCenter(movingImage);

    //Set up initial transform parameters
    TransformType::Pointer transform = TransformType::New();
    transform->SetCenter(centerFixed);
    transform->SetTranslation(centerMoving - centerFixed);
    transform->SetAngle(0.0);

    return transform;
}

void ImageRegistration::PrintRegistrationResults(RegistrationType::Pointer registration, OptimizerType::Pointer optimizer)
{
    //Extract results
    OptimizerType::ParametersType finalParameters = registration->GetLastTransformParameters();

    //Print results to standard out
    std::cout << "Result = " << std::endl;
    std::cout << "   Angle          = " << finalParameters[0] << std::endl;
    std::cout << "   Center X       = " << finalParameters[1]<< std::endl;
    std::cout << "   Center Y       = " << finalParameters[2] << std::endl;
    std::cout << "   Translation X  = " << finalParameters[3] << std::endl;
    std::cout << "   Translation Y  = " << finalParameters[4] << std::endl;
    std::cout << "   Iterations     = " << optimizer->GetCurrentIteration() << std::endl;
    std::cout << "   Metric value   = " << optimizer->GetValue() << std::endl;
}

ImageRegistration::TransformType::Pointer ImageRegistration::DoRegister(ImageType::Pointer fixedImage, ImageType::Pointer movingImage, TransformType::Pointer initialTransform, bool verbose /* = false */)
{
    //Create registration object instances
    MetricType::Pointer metric = MetricType::New();
    OptimizerType::Pointer optimizer = OptimizerType::New();
    InterpolatorType::Pointer interpolator = InterpolatorType::New();
    TransformType::Pointer transform = TransformType::New();
    RegistrationType::Pointer registration = RegistrationType::New();

    //Set up registration
    registration->SetMetric(metric);
    registration->SetOptimizer(optimizer);
    registration->SetInterpolator(interpolator);
    registration->SetTransform(transform);

    registration->SetFixedImage(fixedImage);
    registration->SetMovingImage(movingImage);

    //Set operating region
    fixedImage->Update();
    registration->SetFixedImageRegion(fixedImage->GetBufferedRegion());

    //Set initial transform
    registration->SetInitialTransformParameters(initialTransform->GetParameters());

    //Set up optimizer
    this->SetupOptimizer(optimizer, transform);

    //Start registration
    try
    {
        registration->StartRegistration();
    }
    catch (itk::ExceptionObject &error)
    {
        std::cerr << "ExceptionObject caught!" << std::endl;
        std::cerr << error << std::endl;
        return NULL;
    }

    if (verbose)
    {
        this->PrintRegistrationResults(registration, optimizer);
    }

    //Create the last transform
    TransformType::Pointer finalTransform = TransformType::New();
    finalTransform->SetParameters(registration->GetLastTransformParameters());

    return finalTransform;
}