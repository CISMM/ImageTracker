#ifndef USE_FFTWF
#define USE_FFTWF
#endif

#include <string>

#include "itkCastImageFilter.h"
#include "itkImage.h"
#include "itkRecursiveGaussianImageFilter.h"

#include "DerivativesToSurfaceImageFilter.h"
#include "ImageUtils.h"
#include "Logger.h"

int main(int argc, char** argv)
{
	// Check command line arguments
	if (argc < 5)
	{
		Logger::error << "Usage:\n\t" << argv[0] << " input outDx outDy outSurface" << std::endl;
		Logger::error << "\tNote: output files should support float pixel type (e.g. .vtk, .mha)" << std::endl;
		exit(1);
	}
	
	std::string inFile = argv[1];
	std::string dxFile = argv[2];
	std::string dyFile = argv[3];
	std::string outFile = argv[4];
	
	typedef itk::Image< unsigned short, 2 > InputImageType;
	typedef itk::Image< float, 2 > FloatImageType;
	typedef itk::Image< std::complex< float >, 2 > ComplexImageType;
	
	typedef itk::CastImageFilter< InputImageType, FloatImageType > FloatCasterType;
	typedef itk::CastImageFilter< FloatImageType, InputImageType > ShortCasterType;
	
	typedef itk::RecursiveGaussianImageFilter< FloatImageType, FloatImageType > GaussType;
	typedef DerivativesToSurfaceImageFilter< FloatImageType > SurfaceType;
	
	FloatCasterType::Pointer floatCast = FloatCasterType::New();
	GaussType::Pointer dx = GaussType::New();
	GaussType::Pointer dy = GaussType::New();
	SurfaceType::Pointer surf = SurfaceType::New();
	
	dx->SetDirection(0);	
	dy->SetDirection(1);	
	dx->SetFirstOrder();
	dy->SetFirstOrder();
	dx->SetSigma(2.0);
	dy->SetSigma(2.0);
	
	// Construct pipeline
	dx->SetInput(floatCast->GetOutput());
	dy->SetInput(floatCast->GetOutput());
	surf->SetInputDx(dx->GetOutput());
	surf->SetInputDy(dy->GetOutput());
	
	// Read input
	InputImageType::Pointer inImage = ReadImage<InputImageType>(inFile);
	floatCast->SetInput(inImage);
	
	// Print stats
	PrintImageInfo(floatCast->GetOutput(), "Original image");
	PrintImageInfo(dx->GetOutput(), "Image dx");
	PrintImageInfo(dy->GetOutput(), "Image dy");
	PrintImageInfo(surf->GetOutput(), "Reconstructed image");
	
	// Save output
	WriteImage(dx->GetOutput(), dxFile);
	WriteImage(dy->GetOutput(), dyFile);
	WriteImage(surf->GetOutput(), outFile);
}
