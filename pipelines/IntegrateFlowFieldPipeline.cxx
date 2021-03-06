#include "IntegrateFlowFieldPipeline.h"

#include "itkImageRegionIteratorWithIndex.h"
#include "itkJoinSeriesImageFilter.h"
#include "itkSubtractImageFilter.h"

#include "ImageUtils.h"
#include "Logger.h"
#include "RungeKuttaSolver.h"

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
    
    FlowImageType::Pointer img0 = CopyImage(this->input->GetImage(0));
//     PrintRegionInfo<FlowImageType>(img0->GetLargestPossibleRegion(), "Flow field region");
    
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
    
    FlowImageType::Pointer position = CopyImage(pos0.GetPointer());
    PrintImageInfo<FlowImageType>(position, "Initial position image");
    
    // Save the intitial displacement information
    Logger::debug << function << ": Saving initial displacement image" << std::endl;
    SubtractType::Pointer subtract = SubtractType::New();
    subtract->SetInput2(pos0);
    subtract->SetInput1(position);
    WriteImage(subtract->GetOutput(), this->GetOutputFiles()[0]);
    
    // Extract the first three flow field frames from the input images 
    // (three frames is the maximum needed for linear interpolation at a StepSize < 1 frame).
    JoinFilter::Pointer flows = JoinFilter::New();
    flows->PushBackInput(CopyImage(this->input->GetImage(0)));
    flows->PushBackInput(CopyImage(this->input->GetImage(1)));
    flows->PushBackInput(CopyImage(this->input->GetImage(2)));
    flows->Update();
    
    // Setup displacement integrator
    IntegratorType::Pointer integrate = IntegratorType::New();
    integrate->SetStepSize(this->GetStepSize());
    
    int steps = (int)((this->input->GetImageCount()-1) / this->GetStepSize());
    
    // Integration time = 0
    unsigned int step = 0;
    unsigned int idx = 0;
    double intTime = 0.0;
    bool doContinue = true;
    
    Logger::debug << function << ": Integrating flow" << std::endl;
    this->NotifyProgress(0.0, "Integrating flow");
    for (step = 1; step <= steps; step++) // From frame index idx=0 to the end of the flow field at StepSize intervals:
    {
        Logger::verbose << function << ": intTime =  " << intTime << "\tindex = " << idx << "\tstep = " << step << " of " << steps << std::endl;
        
        // Evaluate the next step using Runge Kutta integration
        integrate->SetInput(position);
        integrate->SetDerivative(flows->GetOutput());
        integrate->SetStartTime(intTime);
        integrate->Update();
        position = integrate->GetOutput();
        
        intTime += this->GetStepSize();
        
        if (intTime >= 1.0) // Whenever the integration time value is greater than 1:
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
                flows->PushBackInput(CopyImage(this->input->GetImage(idx+2)));
                flows->Update();
            }
        }
        
        // Progress notification
        this->NotifyProgress(((double)step/steps), "Integrating flow");
    }
    Logger::debug << function << ": done." << std::endl;
}
