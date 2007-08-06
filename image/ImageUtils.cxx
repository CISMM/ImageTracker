#include "ImageUtils.h"

#include "itkAdaptImageFilter.h"
#include "itkNthElementPixelAccessor.h"

template <>
void PrintImageInfo< ImageTypeV2F2 >(const ImageTypeV2F2* image, const std::string& label, LogStream &logger)
{
    if (label != "")
        logger << label << " info:" << std::endl;
    else
        logger << "Image info: " << std::endl;
    
    ImageTypeV2F2::RegionType region = image->GetLargestPossibleRegion();
    ImageTypeV2F2::PointType origin = image->GetOrigin();
    ImageTypeV2F2::SpacingType spacing = image->GetSpacing();

    logger << "\tOrigin: \t" << origin[0] << ", " << origin[1] << std::endl;
    logger << "\tSpacing:\t" << spacing[0] << ", " << spacing[1] << std::endl;
    logger << "\tRegion: \t" << std::endl;
    logger << "\t\tIndex:\t" << region.GetIndex()[0] << ", " << region.GetIndex()[1] << std::endl;
    logger << "\t\tSize: \t" << region.GetSize()[0] << ", " << region.GetSize()[1] << std::endl;
    
    typedef itk::NthElementPixelAccessor< ImageTypeV2F2::PixelType::ComponentType, ImageTypeV2F2::PixelType  > AccessorType;
    typedef itk::Image< ImageTypeV2F2::PixelType::ComponentType, 2 > ComponentImageType;
    typedef itk::AdaptImageFilter< ImageTypeV2F2, ComponentImageType, AccessorType > ComponentType;
    typedef itk::StatisticsImageFilter< ComponentImageType > StatsType;
        
    ComponentType::Pointer comp = ComponentType::New();
    StatsType::Pointer stats = StatsType::New();
    comp->SetInput(image);
    stats->SetInput(comp->GetOutput());
    
    for (int i = 0; i < 2; i++)
    {
        comp->GetAccessor().SetElementNumber(i);
        comp->Modified(); // have to force the element modification
        stats->Update();
        
        logger << "\tComponent " << i << ":" << std::endl;
        logger << "\tmin:  \t" << (float) stats->GetMinimum() << std::endl;
        logger << "\tmax:  \t" << (float) stats->GetMaximum() << std::endl;
        logger << "\tmean: \t" << (float) stats->GetMean() << std::endl;
        logger << "\tvar:  \t" << (float) stats->GetVariance() << std::endl;
        logger << "\tstd:  \t" << (float) stats->GetSigma() << std::endl;
    }
}
