#include "GlobalRegistrationPipeline.h"

#include "Logger.h"
#include "RegistrationOutput.h"
#include "TransformGroup.h"

GlobalRegistrationPipeline::GlobalRegistrationPipeline(void) :
    init(false),
    index(0)
{
    this->thresholderF = NULL;
    this->thresholderM = NULL;
    this->smootherFx = NULL;
    this->smootherFy = NULL;
    this->smootherMx = NULL;
    this->smootherMy = NULL;
    this->above = NULL;
    this->below = NULL;
    this->sigma = 0.5;
    this->registrar = NULL;
}

GlobalRegistrationPipeline::~GlobalRegistrationPipeline(void)
{
    delete this->registrar;
}

void GlobalRegistrationPipeline::InitializePipeline()
{
    if (this->inputFiles.size() != 0)
    {
        this->reader = ReaderType(this->inputFiles);
        this->index = 0;

        this->thresholderF = ThresholdType::New();
        this->thresholderM = ThresholdType::New();
        this->smootherFx = SmoothType::New();
        this->smootherFy = SmoothType::New();
        this->smootherMx = SmoothType::New();
        this->smootherMy = SmoothType::New();
        
        this->smootherFx->SetDirection(0);
        this->smootherMx->SetDirection(0);
        this->smootherFx->SetDirection(1);
        this->smootherMx->SetDirection(1);

        this->thresholderF->SetInput(this->reader[this->index]);
        this->thresholderM->SetInput(this->reader[this->index+1]);

        this->smootherFx->SetInput(this->thresholderF->GetOutput());
        this->smootherFy->SetInput(this->smootherFx->GetOutput());
        this->smootherMx->SetInput(this->thresholderM->GetOutput());
        this->smootherMy->SetInput(this->smootherMx->GetOutput());
        
        this->transforms = TransformVector();
        this->registrar = new RegistrationHelper();

        this->init = true;
        this->UpdatePipeline();
    }
    else
    {
        this->init = false;
    }
}

void GlobalRegistrationPipeline::UpdatePipeline()
{
    if (!this->init)
    {
        this->InitializePipeline();
    }
    else
    {
        if (this->above && this->below)
        {
            this->thresholderF->ThresholdOutside(*this->below, *this->above);
            this->thresholderM->ThresholdOutside(*this->below, *this->above);
        }
        else if (this->above)
        {
            this->thresholderF->ThresholdAbove(*this->above);
            this->thresholderM->ThresholdAbove(*this->above);
        }
        else if (this->below)
        {
            this->thresholderF->ThresholdBelow(*this->below);
            this->thresholderM->ThresholdBelow(*this->below);
        }
        else
        {
            GlobalRegistrationPipeline::ImageType::PixelType min = 0;
            this->thresholderF->ThresholdBelow(min);
            this->thresholderM->ThresholdBelow(min);
        }

        this->smootherFx->SetSigma(this->sigma);
        this->smootherFy->SetSigma(this->sigma);
        this->smootherMx->SetSigma(this->sigma);
        this->smootherMy->SetSigma(this->sigma);
    }
}

GlobalRegistrationPipeline::TransformType::Pointer GlobalRegistrationPipeline::UpdateOne()
{
    TransformType::Pointer result = NULL;

    if (!this->init)
    {
        this->InitializePipeline();
    }

    if (this->init && this->index < (this->reader.size()-1))
    {
        this->thresholderF->SetInput(this->reader[this->index]);
        this->thresholderM->SetInput(this->reader[this->index+1]);

        result = this->registrar->Register(
            this->GetCurrentFixedImage(), 
            this->GetCurrentMovingImage());

        this->transforms.push_back(result);
    }

    return result;
}

GlobalRegistrationPipeline::TransformVector * GlobalRegistrationPipeline::UpdateAll()
{
    while (this->UpdateOne()); // Repeat until a NULL result is returned

    return &this->transforms;
}

bool GlobalRegistrationPipeline::IsRegistrationFinished()
{
    return (this->init && !(this->index < this->reader.size()-1));
}

void GlobalRegistrationPipeline::SaveTransforms()
{
    if (this->transforms.size() != 0)
    {
        TransformGroup::SaveTransforms(&this->transforms, this->transformFile);
    }
}

void GlobalRegistrationPipeline::SaveImages()
{
    if (!(this->transforms.size() == 0 || this->inputFiles.size() == 0 || this->outputFiles.size() == 0))
    {
        RegistrationOutput regOut(
            this->inputFiles, 
            this->transforms, 
            this->outputFiles, 
            RegistrationOutput::COMPOSE_PRE);
        regOut.Update();
    }
}

void GlobalRegistrationPipeline::SetThresholdAbove(ThresholdType::PixelType* above)
{
    this->above = above;
    this->UpdatePipeline();
}

void GlobalRegistrationPipeline::SetThresholdBelow(ThresholdType::PixelType* below)
{
    this->below = below;
    this->UpdatePipeline();
}

void GlobalRegistrationPipeline::SetSmoothSigma(double sigma)
{
    this->sigma = sigma;
    this->UpdatePipeline();
}

void GlobalRegistrationPipeline::SetInput(const FileSet& input)
{
    this->inputFiles = input;
    this->InitializePipeline();
}

void GlobalRegistrationPipeline::SetOutput(const FileSet& output)
{
    this->outputFiles = output;
}

void GlobalRegistrationPipeline::SetTransformFile(std::string transformFile)
{
    this->transformFile = transformFile;
}

const FileSet& GlobalRegistrationPipeline::GetInput()
{
    return this->inputFiles;
}

const FileSet& GlobalRegistrationPipeline::GetOutput()
{
    return this->outputFiles;
}

std::string GlobalRegistrationPipeline::GetTransformFile()
{
    return this->transformFile;
}

GlobalRegistrationPipeline::ImageType::Pointer GlobalRegistrationPipeline::GetCurrentFixedImage()
{
    this->smootherFy->Update();
    return this->smootherFy->GetOutput();
}

GlobalRegistrationPipeline::ImageType::Pointer GlobalRegistrationPipeline::GetCurrentMovingImage()
{
    this->smootherMy->Update();
    return this->smootherMy->GetOutput();
}

const GlobalRegistrationPipeline::TransformVector& GlobalRegistrationPipeline::GetTransforms()
{
    return this->transforms;
}

// RegistrationHelper
RegistrationHelper::RegistrationHelper()
{
    this->registration = NULL;
    this->metric = NULL;
    this->optimizer = NULL;
    this->interpolator = NULL;

    this->Initialize();
}

RegistrationHelper::~RegistrationHelper(void)
{
}

void RegistrationHelper::Initialize()
{
    this->metric = MetricType::New();
    this->optimizer = OptimizerType::New();
    this->interpolator = InterpolatorType::New();
    this->registration = RegistrationType::New();

    TransformType::Pointer transform = TransformType::New();

    this->registration->SetMetric(this->metric);
    this->registration->SetOptimizer(this->optimizer);
    this->registration->SetInterpolator(this->interpolator);
    this->registration->SetTransform(transform);

    this->SetupOptimizer();
}

RegistrationHelper::TransformType::Pointer RegistrationHelper::Register(
    ImageType::Pointer fixed, 
    ImageType::Pointer moving)
{
    return this->Register(fixed, moving, this->DefaultInitialTransform(fixed, moving));
}

RegistrationHelper::TransformType::Pointer RegistrationHelper::Register(
    ImageType::Pointer fixed, 
    ImageType::Pointer moving, 
    TransformType::Pointer initialTransform)
{
    this->registration->SetInitialTransformParameters(initialTransform->GetParameters());
    this->registration->SetFixedImage(fixed);
    this->registration->SetMovingImage(moving);

    try
    {
        this->registration->StartRegistration();
    }
    catch (itk::ExceptionObject &error)
    {
        Logger::logError("ExceptionObject caught!");
        Logger::logError(error.GetDescription());
        return NULL;
    }

    char text[200];
    sprintf(text, "Optimizer: iterations => %i; metric => %8.4f", this->optimizer->GetCurrentIteration(), this->optimizer->GetValue());
    Logger::logVerbose(text);

    TransformType::Pointer finalTransform = TransformType::New();
    finalTransform->SetParameters(this->registration->GetLastTransformParameters());

    return finalTransform;
}

void RegistrationHelper::SetupOptimizer()
{
    //Set up scaling for the different units of the transformation, keeping in mind
    //that angles of rotation are typically small compared to the translation.
    OptimizerType::ScalesType optimizerScales(TransformType::New()->GetNumberOfParameters());

    const double translationScale = 1.0 / 1000.0;

    optimizerScales[0] = 1.0;
    optimizerScales[1] = translationScale;
    optimizerScales[2] = translationScale;
    optimizerScales[3] = translationScale;
    optimizerScales[4] = translationScale;

    optimizer->SetScales(optimizerScales);

    //Set up step lengths
    this->optimizer->SetMaximumStepLength(0.5);
    this->optimizer->SetMinimumStepLength(0.01);
    this->optimizer->SetNumberOfIterations(200);
}

RegistrationHelper::TransformType::InputPointType RegistrationHelper::FindImageCenter(const ImageType::Pointer image)
{
    const ImageType::SpacingType& spacing = image->GetSpacing();
    const ImageType::PointType& origin = image->GetOrigin();

    ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();

    TransformType::InputPointType center;
    center[0] = origin[0] + spacing[0] * size[0] / 2.0;
    center[1] = origin[1] + spacing[1] * size[1] / 2.0;

    return center;
}

RegistrationHelper::TransformType::Pointer RegistrationHelper::DefaultInitialTransform(ImageType::Pointer fixed, ImageType::Pointer moving)
{
    fixed->Update();
    moving->Update();

    //Find the centers of the images        
    TransformType::InputPointType centerFixed =  this->FindImageCenter(fixed);
    TransformType::InputPointType centerMoving = this->FindImageCenter(moving);

    //Set up initial transform parameters
    TransformType::Pointer transform = TransformType::New();
    transform->SetCenter(centerFixed);
    transform->SetTranslation(centerMoving - centerFixed);
    transform->SetAngle(0.0);

    return transform;
}
