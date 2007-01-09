#include <string>

#include "itkImage.h"
#include "itkShiftScaleImageFilter.h"
#include "itkStatisticsImageFilter.h"

#include "ImageUtils.h"
#include "Logger.h"
#include "PadFunctorImageFilter.h"

int main(int argc, char** argv)
{
    // Check arguments
    if (argc < 5)
    {
        Logger::error << "Usage:\n\t" << argv[0] << " input output edgeValue padRadius" << std::endl;
        exit(1);
    }
    
    Logger::verbose << "Parsing parameters" << std::endl;
    std::string inFile = argv[1];
    std::string outFile = argv[2];
    float edgeValue = atof(argv[3]);
    int padRadius = atoi(argv[4]);
    
    Logger::verbose << "Defining types" << std::endl;
    const unsigned int Dimension = 2;
    typedef itk::Image< float, Dimension > InputImageType;
    typedef itk::Image< float, Dimension > OutputImageType;
    typedef PadFunctorImageFilter< InputImageType, OutputImageType, CosFunctor > PadType;
    typedef itk::StatisticsImageFilter< OutputImageType > StatsType;
    typedef itk::ShiftScaleImageFilter< OutputImageType, OutputImageType > ScaleType;
    
    Logger::verbose << "Initializing pipeline" << std::endl;
    PadType::Pointer pad = PadType::New();
    StatsType::Pointer stats = StatsType::New();
    ScaleType::Pointer scale = ScaleType::New();
    
    for (unsigned int i = 0; i < Dimension; i++)
    {
        pad->SetPadRadius(i, padRadius);
    }
    pad->SetBoundaryValue(edgeValue);
    
    Logger::verbose << "Reading input image" << std::endl;
    pad->SetInput(ReadImage<InputImageType>(inFile));
    
    WriteImage(pad->GetOutput(), outFile);
    
//     Logger::verbose << "Padding image" << std::endl;
//     pad->Update();
//     
//     Logger::verbose << "Scaling output" << std::endl;
//     stats->SetInput(pad->GetOutput());
//     stats->Update();
//     
//     scale->SetInput(pad->GetOutput());
//     scale->SetScale(1.0 / stats->GetMaximum());
//     
//     Logger::verbose << "Writing output" << std::endl;
//     WriteImage(scale->GetOutput(), outFile);
}
