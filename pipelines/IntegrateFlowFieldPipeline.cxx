#include "IntegrateFlowFieldPipeline.h"

#include "itkImageDuplicator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkJoinSeriesImageFilter.h"
#include "itkSubtractImageFilter.h"

#include "ImageUtils.h"
#include "Logger.h"

void IntegrateFlowFieldPipeline::Update()
{
    std::string function("IntegrateFlowFieldPipeline::Update()");
    Logger::debug << function << ": Entering." << std::endl;
    this->NotifyProgress(0.0, "Initializing");
    
    typedef itk::ImageRegionIteratorWithIndex< FlowImageType > FlowIterator;
    typedef itk::JoinSeriesImageFilter<FlowImageType, FlowSeriesImageType> JoinFilter;
    typedef RungeKuttaSolver< FlowImageType, FlowImageType, FlowSeriesImageType > IntegratorType;
    typedef itk::ImageDuplicator< FlowImageType > CopyType;
    typedef itk::SubtractImageFilter< FlowImageType, FlowImageType, FlowImageType > SubtractType;
    
    FlowImageType* img0 = dynamic_cast<FlowImageType*>(this->input->GetImage(0));
    PrintRegionInfo<FlowImageType>(img0->GetLargestPossibleRegion(), "Flow field region");
    
    // Initialize position information by creating a vector-image where each vector value
    // is determined by the image grid position of the cell.
    FlowImageType::Pointer pos0 = FlowImageType::New();
    FlowImageType::RegionType region = img0->GetLargestPossibleRegion();
    FlowImageType::SpacingType spacing = img0->GetSpacing();
    FlowImageType::PointType origin = img0->GetOrigin();
    pos0->SetRegions(region);
    pos0->SetSpacing(spacing);
    pos0->SetOrigin(origin);
    pos0->Allocate();
    
    Logger::debug << function << ": Initializing position image" << std::endl;
    FlowIterator posIt(pos0, region);
    for (posIt.GoToBegin(); !posIt.IsAtEnd(); ++posIt)
    {
        FlowImageType::IndexType index(posIt.GetIndex());
        FlowImageType::PixelType pos;
        for (int d = 0; d < FlowImageType::ImageDimension; d++)
        {
            pos[d] = index[d] * spacing[d] + origin[d];
        }
        posIt.Set(pos);
    }
    
    CopyType::Pointer copy = CopyType::New();
    FlowImageType::Pointer position = FlowImageType::New();
    copy->SetInputImage(pos0);
    copy->Update();
    position = copy->GetOutput();
    PrintRegionInfo<FlowImageType>(position->GetLargestPossibleRegion(), "Initial position region");
    
    // Save the intitial displacement information
    Logger::debug << function << ": Saving initial displacement image" << std::endl;
    SubtractType::Pointer subtract = SubtractType::New();
    subtract->SetInput2(pos0);
    subtract->SetInput1(position);
    WriteImage(subtract->GetOutput(), this->GetOutputFiles()[0]);
    
    // Extract the first three flow field frames from the input images 
    // (three frames is the maximum needed for linear interpolation at a StepSize < 1 frame).
    JoinFilter::Pointer flows = JoinFilter::New();
    flows->PushBackInput(dynamic_cast<FlowImageType*>(this->input->GetImage(0)));
    flows->PushBackInput(dynamic_cast<FlowImageType*>(this->input->GetImage(1)));
    flows->PushBackInput(dynamic_cast<FlowImageType*>(this->input->GetImage(2)));
    
    // Setup displacement integrator
    IntegratorType::Pointer integrate = IntegratorType::New();
    integrate->SetStepSize(this->GetStepSize());
    
    int steps = (int)(this->input->size() / this->GetStepSize());
    
    // Integration time = 0
    unsigned int step = 0;
    unsigned int idx = 0;
    double intTime = -this->GetStepSize();
    bool doContinue = true;
    
    Logger::debug << function << ": Integrating flow" << std::endl;
    this->NotifyProgress(0.0, "Integrating flow");
    while (doContinue) // From frame index idx=0 to the end of the flow field at StepSize intervals:
    {
        intTime += this->GetStepSize();
        
        // Evaluate the next step using Runge Kutta integration
        integrate->SetInput(position);
        integrate->SetDerivative(flows->GetOutput());
        integrate->SetStartTime(intTime);
        integrate->Update();
        position = integrate->GetOutput();
        
        doContinue == !(idx + 3 > this->input->size() && intTime > 1);
        
        if (intTime > 1.0) // Whenever the integration time value is greater than 1:
        {
            // Subtract 1 from the integration time value
            intTime -= 1.0;
            // Increase the frame index by 1
            idx++;
            
            // Save the current displacement field image
            subtract->SetInput1(position);
            WriteImage(subtract->GetOutput(), this->GetOutputFiles()[idx]);
            
            // Push the next flow field into the stack, and pop the first flow field off
            if (doContinue)
            {
                flows->PopFrontInput();
                flows->PushBackInput(dynamic_cast<FlowImageType*>(this->input->GetImage(idx+2)));
            }
        }
        
        // Progress notification
        this->NotifyProgress(((double)step++/steps), "Integrating flow");
    }
    Logger::debug << function << ": done." << std::endl;
}
