
#include <string>

#include "itkCastImageFilter.h"
#include "itkImage.h"
#include "itkImageDuplicator.h"
#include "itkImageSeriesReader.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkSubtractImageFilter.h"
#include "itkVector.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "ImageUtils.h"
#include "Logger.h"
#include "RungeKuttaSolver.h"

/**
 * Integrates the position of each image location within a
 * time-varying flow field.  The input flow field specifies
 * velocity, v(x,y,t), at each image location, (x,y), over time.
 * The position, p(x,y,t), of a weightless particle in this flow
 * field, then is given by the solution to dp(x,y,t)/dt = v(x,y,t).
 * 
 * Given a series of flow field images, a starting time, end time,
 * and time step, we estimate the solution to the above differential
 * equation using a 4th order Runge-Kutta solver (RK4).
 */
int main(int argc, char** argv)
{
    std::string function("IntegrateFlowField");
    // Check argument count
    if (argc < 10)
    {
        Logger::error << "Usage:\n\t" << argv[0] << " dir formatIn start stop timeStart timeStop timeStep initOut formatOut" << std::endl;
        exit(1);
    }
    
    std::string dir         = argv[1];
    std::string formatIn    = argv[2];
    int start               = atoi(argv[3]);
    int end                 = atoi(argv[4]);
    float timeStart         = atof(argv[5]);
    float timeEnd           = atof(argv[6]);
    float timeStep          = atof(argv[7]);
    std::string initOutFile = argv[8];
    std::string formatOut   = argv[9];
    
    typedef itk::Vector< float, 2 > VectorPixelType;
    typedef itk::Image< VectorPixelType, 3 > FlowFieldType;
    typedef itk::ImageSeriesReader< FlowFieldType > FlowReaderType;
    
    typedef itk::Image< VectorPixelType, 2 > PositionFieldType;
    typedef itk::ImageRegionIteratorWithIndex< PositionFieldType > PositionIteratorType;
    
    typedef RungeKuttaSolver< PositionFieldType, PositionFieldType, FlowFieldType > SolverType;
    typedef itk::ImageDuplicator< PositionFieldType > CopyType;
    typedef itk::SubtractImageFilter< PositionFieldType, PositionFieldType, PositionFieldType > SubtractType;
    
    // Load flow field
    Logger::verbose << function << ": Loading flow field" << std::endl;
    FileSet flowFiles(FilePattern(dir, formatIn, start, end));
    FlowReaderType::Pointer reader = FlowReaderType::New();
    reader->SetFileNames(flowFiles.GetFileNames());
    reader->Update();
    PrintRegionInfo<FlowFieldType>(reader->GetOutput()->GetLargestPossibleRegion(), "Flow field region");
    
    // Create initial position (initial conditions)
    Logger::verbose << function << ": Creating initial condition image" << std::endl;
    FlowFieldType::SizeType flowSize = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
    PositionFieldType::SizeType size;
    PositionFieldType::IndexType index;
    for (unsigned int d = 0; d < PositionFieldType::ImageDimension; d++)
    {
        size[d] = flowSize[d];
        index[d] = 0;
    }
    PositionFieldType::RegionType region;
    region.SetSize(size);
    region.SetIndex(index);
    PositionFieldType::Pointer initial = PositionFieldType::New();
    initial->SetRegions(region);
    initial->Allocate();
    PrintRegionInfo<PositionFieldType>(initial->GetLargestPossibleRegion(), "Initial position region");
    
    // Fill initial conditions with image indices
    Logger::verbose << function << ": Filling initial conditions with image indices." << std::endl;
    PositionIteratorType posIt(initial, region);
    for (posIt.GoToBegin(); !posIt.IsAtEnd(); ++posIt)
    {
        PositionFieldType::PixelType pixel;
        PositionFieldType::IndexType index(posIt.GetIndex());
        for (unsigned int d = 0; d < PositionFieldType::ImageDimension; d++)
        {
            pixel[d] = index[d];
        }
        posIt.Set(pixel);
    }
    WriteImage<PositionFieldType>(initial, initOutFile);
    
    // Create RungeKutter solver
    Logger::verbose << function << ": Setting up RK4 solver" << std::endl;
    SolverType::Pointer solver = SolverType::New();
    CopyType::Pointer copy = CopyType::New();
    SubtractType::Pointer subtract = SubtractType::New();
    subtract->SetInput2(initial);
    
    PositionFieldType::Pointer current = PositionFieldType::New();
    copy->SetInputImage(initial);
    copy->Update();
    current = copy->GetOutput();
    
    solver->SetDerivative(reader->GetOutput());
    solver->SetStepSize(timeStep);
    
    int steps = int ((timeEnd - timeStart) / timeStep);
    for (int i = 0; i < steps; i++)
    {
        // Solve the next time step
        double time = timeStart + i * timeStep;
        Logger::verbose << function << ": Time: " << time << " -> " << (time + timeStep) << std::endl;
        solver->SetInput(current);
        solver->SetStartTime(time);
        solver->Update();
        
        // Copy output to current position
        copy->SetInputImage(solver->GetOutput());
        copy->Update();
        current = copy->GetOutput();
        
        // Write to a file
        subtract->SetInput1(current);
        char file[120];
        sprintf(file, formatOut.c_str(), i + start);
        std::string filestr(file);
        WriteImage<PositionFieldType>(subtract->GetOutput(), filestr);
    }
}
