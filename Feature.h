#pragma once
#include "CommonTypes.h"

class Feature
{
public:
    
    typedef CommonTypes::ImageType ImageType;
    
    Feature(void) {}
    
    Feature(ImageType::IndexType loc, ImageType::PixelType score)
    {
        this->location = loc;
        this->harrisScore = score;
    }
    
    virtual ~Feature(void) {}

    bool operator<(Feature other)
    {
        return this->harrisScore < other.harrisScore;
    }

    bool operator<=(Feature other)
    {
        return this->harrisScore <= other.harrisScore;
    }

    bool operator>(Feature other)
    {
        return this->harrisScore > other.harrisScore;
    }

    bool operator>=(Feature other)
    {
        return this->harrisScore >= other.harrisScore;
    }

    bool operator==(Feature other)
    {
        return this->harrisScore == other.harrisScore;
    }

    ImageType::PixelType getHarrisScore()
    {
        return this->harrisScore;
    }

    ImageType::IndexType getLocation()
    {
        return this->location;
    }

    double getScale()
    {
        return this->scale;
    }

    void setHarrisScore(ImageType::PixelType score)
    {
        this->harrisScore = score;
    }
    
    void setLocation(ImageType::IndexType loc)
    {
        this->location = loc;
    }
    
    void setScale(double scale)
    {
        this->scale = scale;
    }

private:
    ImageType::IndexType location;
    ImageType::PixelType harrisScore;
    double scale;
};
