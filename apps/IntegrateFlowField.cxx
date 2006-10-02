
#include <string>

#include "itkCastImageFilter.h"
#include "itkImage.h"
#include "itkImageDuplicator.h"
#include "itkImageSeriesReader.h"
#include "itkImageRegionIteratorWithIndex.h"
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
    // Check argument count
    if (argc < 10)
    {
        Logger::error << "Usage:\n\t" << argv[0] << " dir formatIn start stop timeStart timeStop timeStep initOut posOut" << std::endl;
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
    std::string posOutFile  = argv[9];
    
    typedef itk::Vector< float, 2 > VectorPixelType;
    typedef itk::Image< VectorPixelType, 3 > FlowFieldType;
    typedef itk::ImageSeriesReader< FlowFieldType > FlowReaderType;
    
    typedef itk::Image< VectorPixelType, 2 > PositionFieldType;
    typedef itk::ImageRegionIteratorWithIndex< PositionFieldType > PositionIteratorType;
    
    typedef RungeKuttaSolver< PositionFieldType, PositionFieldType, FlowFieldType > SolverType;
    typedef itk::ImageDuplicator< PositionFieldType > CopyType;
    
    // Load flow field
    Logger::verbose << "Loading flow field" << std::endl;
    FileSet flowFiles(FilePattern(dir, formatIn, start, end));
    FlowReaderType::Pointer reader = FlowReaderType::New();
    reader->SetFileNames(flowFiles.GetFileNames());
    reader->Update();
    
    // Create initial position (initial conditions)
    Logger::verbose << "Creating initial condition image" << std::endl;
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
    
    // Fill initial conditions with image indices
    Logger::verbose << "Filling initial conditions with image indices." << std::endl;
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
    Logger::verbose << "Setting up RK4 solver" << std::endl;
    SolverType::Pointer solver = SolverType::New();
    CopyType::Pointer copy = CopyType::New();
    
    solver->SetInput(initial);
    solver->SetDerivative(reader->GetOutput());
    solver->SetStartTime(timeStart);
    solver->SetStepSize(timeStep);
    
    Logger::verbose << "Solving first time step." << std::endl;
    Logger::verbose << "\tTime: " << timeStart << " -> " << (timeStart+timeStep) << std::endl;
    solver->Update();
    
    // Solve for position over all image steps
    Logger::verbose << "Solving over all time steps" << std::endl;
    int steps = int ((timeEnd - timeStart) / timeStep);
    for (int i = 1; i < steps; i++)
    {
        Logger::verbose << "\tTime: " << (timeStart + i * timeStep) << " -> " << (timeStart + (i+1) * timeStep) << std::endl;
        copy->SetInputImage(solver->GetOutput());
        copy->Update(); // This updates the input to the solver
        solver->SetInput(copy->GetOutput());
        solver->SetStartTime(timeStart + i * timeStep);
        solver->Update();
    }
    
    Logger::verbose << "Writing result" << std::endl;
    WriteImage(solver->GetOutput(), posOutFile);
}
