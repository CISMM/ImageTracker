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

    logger << "\tOrigin: \t" << origin[0] << ", " << origin[1] << std::endl;
    logger << "\tSpacing:\t" << spacing[0] << ", " << spacing[1] << std::endl;
    logger << "\tRegion: \t" << std::endl;
    logger << "\t\tIndex:\t" << region.GetIndex()[0] << ", " << region.GetIndex()[1] << std::endl;
    logger << "\t\tSize: \t" << region.GetSize()[0] << ", " << region.GetSize()[1] << std::endl;
}
