#include "ImageStatistics.h"

#include <cmath>

#include "itkImageRegionIterator.h"

ImageStatistics::ImageStatistics(void)
{
}

ImageStatistics::~ImageStatistics(void)
{
}

void ImageStatistics::SetImage(ImageType::Pointer image)
{
    this->image = image;
    this->DoCompute();
}

int ImageStatistics::GetCount()
{
    return this->count;
}

double ImageStatistics::GetDeviation()
{
    return this->deviation;
}

double ImageStatistics::GetMean()
{
    return this->mean;
}

double ImageStatistics::GetMinimum()
{
    return this->minimum;
}

double ImageStatistics::GetMaximum()
{
    return this->maximum;
}

void ImageStatistics::DoCompute()
{
    typedef itk::ImageRegionIterator< ImageType > IteratorType;
    ImageType::RegionType region = this->image->GetLargestPossibleRegion();

    IteratorType imageIt(this->image, region);
    
    // Calculate the mean
    double sum = 0;
    int count = 0;
    ImageStatistics::ImageType::PixelType pixel;
    double length, component;
    double min, max;

    for (imageIt.GoToBegin(); !imageIt.IsAtEnd(); ++imageIt)
    {
        pixel = imageIt.Get();
        length = 0;
        for (int i = 0; i < pixel.GetNumberOfComponents(); i++)
        {
            component = (double) pixel[i];
            length += component * component;
        }

        length = sqrt(length);
        sum += length;

        if (count == 0) // initialize min/max
        {
            min = length;
            max = length;
        }
        else
        {
            if (length < min)
                min = length;
            if (length > max)
                max = length;
        }
        count++;

        //if (count % 512 == 0)
        //{
        //    std::cout << "Pixel[" << count << "]: " << pixel << std::endl;
        //}
    }

    this->count = count;
    this->mean = sum / (double) count;
    this->minimum = min;
    this->maximum = max;

    // Caclulate the standard deviation
    sum = 0;
    double meanDevSqd;
    for (imageIt.GoToBegin(); !imageIt.IsAtEnd(); ++imageIt)
    {
        pixel = imageIt.Get();
        length = 0;
        for (int i = 0; i < pixel.GetNumberOfComponents(); i++)
        {
            component = (double) pixel[i];
            length += component * component;
        }

        length = sqrt(length);
        meanDevSqd = (length - this->mean) * (length - this->mean);
        sum += meanDevSqd;
    }

    double sample = 1.0 / (double) count;
    this->deviation = sqrt(sample * sum);
}