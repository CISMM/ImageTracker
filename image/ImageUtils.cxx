#include "ImageUtils.h"

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

    logger << "\tOrigin:  " << origin[0] << ", " << origin[1] << std::endl;
    logger << "\tSpacing: " << spacing[0] << ", " << spacing[1] << std::endl;
    logger << "\tRegion:  " << std::endl;
    logger << "\t  Index: " << region.GetIndex()[0] << ", " << region.GetIndex()[1] << std::endl;
    logger << "\t   Size: " << region.GetSize()[0] << ", " << region.GetSize()[1] << std::endl;
}
