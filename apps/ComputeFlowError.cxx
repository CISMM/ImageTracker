
#include <string>

#include "itkImage.h"
#include "itkSquaredDifferenceImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "itkVector.h"
#include "itkWarpImageFilter.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"
#include "Logger.h"
#include "NaryMeanImageFilter.h"
#include "PeakSNRImageToImageMetric.h"

/**
 * Computes the squared difference between original images in a video sequence 
 * and images warped with computed flow fields.  This gives an estimate of
 * where the flow computation is more accurate and where it has failed.
 *
 * The flow computation between two images, I1 and I2, estimates how far each
 * pixel in I1 moves to explain the image intensities in I2.  The result is a
 * dense vector displacement field, d, where I2(x+d) = I1(x).  One can compare
 * the success of a flow computation, then, by inverse warping the image I2 by
 * the displacement field d.  Inverse warping is used so that there are no tears
 * in the warped image.  The warped I2 can then be compared to I1 to determine
 * where the displacement field does not well describe the image intensities
 * in the two images.
 */
int main(int argc, char** argv)
{
    std::string function("ComputeFlowError");
    if (argc < 8)
    {
        Logger::warning << "Usage:\n\t" << argv[0] << " dir imgFormat flowFormat start end errFormat meanErr" << std::endl;
        exit(1);
    }
    
    typedef itk::Image< unsigned short, 2 > InputImageType;
    typedef itk::Image< float, 2 > InternalImageType;
    typedef itk::Image< itk::Vector< float, 2 >, 2 > FlowImageType;
    typedef itk::WarpImageFilter< InternalImageType, InternalImageType, FlowImageType > WarpType;
    typedef itk::SquaredDifferenceImageFilter< InternalImageType, InternalImageType, InternalImageType > ErrorType;
    typedef NaryMeanImageFilter<InternalImageType, InternalImageType> MeanType;
    typedef itk::ThresholdImageFilter< InternalImageType > ThresholdType;
    typedef PeakSNRImageToImageMetric< InternalImageType, InternalImageType > PSNRType;
    
    Logger::verbose << function << ": Parsing inputs" << std::endl;
    std::string dir(argv[1]);
    std::string imgFormat(argv[2]);
    std::string flowFormat(argv[3]);
    int start = atoi(argv[4]);
    int end = atoi(argv[5]);
    std::string errFormat(argv[6]);
    std::string meanFile(argv[7]);
    
    FileSet imageFiles(FilePattern(dir, imgFormat, start, end));
    FileSet flowFiles(FilePattern(dir, flowFormat, start, end-1));
    FileSet errorFiles(FilePattern(dir, errFormat, start, end-1));
    
    Logger::verbose << function << ": Setting up pipeline" << std::endl;
    ImageSetReader<InputImageType, InternalImageType> images(imageFiles);
    ImageSetReader<FlowImageType, FlowImageType > flows(flowFiles);
    
    WarpType::Pointer warp = WarpType::New();
    ErrorType::Pointer error = ErrorType::New();
    MeanType::Pointer mean = MeanType::New();
    ThresholdType::Pointer threshold = ThresholdType::New();
    PSNRType::Pointer psnr = PSNRType::New();
    
    images[0]->Update();
    warp->SetOutputOrigin(images[0]->GetOrigin());
    warp->SetOutputSpacing(images[0]->GetSpacing());
    
    Logger::verbose << function << ": Computing error" << std::endl;
    for (int i = 0; i < flows.size(); i++)
    {
        warp->SetInput(images[i+1]);
        warp->SetDeformationField(flows[i]);
        error->SetInput1(images[i]);
        error->SetInput2(warp->GetOutput());
        WriteImage<InternalImageType, InputImageType>(error->GetOutput(), errorFiles[i]);
        mean->PushBackInput(error->GetOutput());
        
//         PrintImageInfo<InternalImageType>(images[i], "Reference image");
//         PrintImageInfo<InternalImageType>(images[i+1], "Moving image");
//         PrintImageInfo(warp->GetOutput(), "Warped image");
//         PrintImageInfo<FlowImageType>(flows[i], "Deformation image");
        
        char file[80];
        sprintf(file, "warp-%04d.tif", i + start);
        WriteImage<InternalImageType, InputImageType>(warp->GetOutput(), std::string(file));
        
        char msg[80];
        sprintf(msg, "Error image %4d", i);
        PrintImageInfo(error->GetOutput(), msg);
        
        // PSNR
        psnr->SetSourceImage(images[i]);
        psnr->SetReconstructImage(warp->GetOutput());
        Logger::verbose << function << ": RMSE\t" << i << " - w:\t" << psnr->GetRMSE() << std::endl;
        Logger::verbose << function << ": PSNR\t" << i << " - w:\t" << psnr->GetPSNR() << std::endl;
        psnr->SetReconstructImage(images[i+1]);
        Logger::verbose << function << ": RMSE\t" << i << " - " << (i+1) << ":\t" << psnr->GetRMSE() << std::endl;
        Logger::verbose << function << ": PSNR\t" << i << " - " << (i+1) << ":\t" << psnr->GetPSNR() << std::endl;
    }
    
    Logger::verbose << "Computing mean error" << std::endl;
    mean->Update();
    
    PrintImageInfo(mean->GetOutput(), "Mean error");
    
    threshold->SetInput(mean->GetOutput());
    threshold->SetOutsideValue(std::numeric_limits<InputImageType::PixelType>::max());
    threshold->ThresholdAbove(std::numeric_limits<InputImageType::PixelType>::max());
    WriteImage<InternalImageType, InputImageType>(threshold->GetOutput(), meanFile);
    
    Logger::verbose << function << ": Done." << std::endl;
}
