#include <iostream>

#include "BlurFilterSchedule.h"

BlurFilterSchedule::BlurFilterSchedule(
    double minDeviation     /* = 0.0 */, 
    double maxDeviation     /* = 3.0 */,
    int levels              /* = 3 */, 
    double maxKernelSize    /* = 10.0 */)
{
    this->minDeviation = minDeviation;
    this->maxDeviation = maxDeviation;
    this->numberOfLevels = levels;
    this->maxKernelSize = maxKernelSize;

    this->resetSchedule();
}

BlurFilterSchedule::~BlurFilterSchedule(void)
{
}

BlurFilterSchedule* BlurFilterSchedule::Clone()
{
    return new BlurFilterSchedule(
        this->minDeviation, this->maxDeviation, this->numberOfLevels, this->maxKernelSize);
}

void BlurFilterSchedule::SetMaxDeviation(double maxDeviation)
{
    this->maxDeviation = maxDeviation;
    this->resetSchedule();
}

void BlurFilterSchedule::SetMaxKernelSize(double size)
{
    this->maxKernelSize = size;
    this->resetSchedule();
}

void BlurFilterSchedule::SetNumberOfLevels(int levels)
{
    this->numberOfLevels = levels;
    this->resetSchedule();
}

void BlurFilterSchedule::SetSourceImage(const ImageType::Pointer image)
{
    this->sourceImage = image;
    this->resetSchedule();
}

double BlurFilterSchedule::GetCurrentDeviation(void)
{
    //Current level can range from numberOfLevels down to 1.
    double deviation = this->minDeviation + (this->currentLevel - 1) * this->stepSize;
    //std::cout << "Deviation at level " << this->currentLevel << " -> " << deviation << std::endl;
    return deviation;
}

int BlurFilterSchedule::GetCurrentLevel(void)
{
    return this->currentLevel;
}

bool BlurFilterSchedule::HasMoreLevels(void)
{
    return (!this->blurAccessed || (this->currentLevel > 1));
}

BlurFilterSchedule::ImageType::Pointer BlurFilterSchedule::CurrentBlurredImage(void)
{
    this->blurAccessed = true;
    //Use a blur filter, if our standard deviation is not 0:
    double currentDeviation = this->GetCurrentDeviation();
    if (currentDeviation > 0)
    {
        BlurFilterType::Pointer blurFilter = BlurFilterType::New();
        blurFilter->SetVariance(currentDeviation);
        blurFilter->SetMaximumKernelWidth(this->maxKernelSize);
        blurFilter->SetInput(this->sourceImage);
        blurFilter->Update();
        return blurFilter->GetOutput();
    }

    //else -- no blurring required
    return this->sourceImage;
}

BlurFilterSchedule::ImageType::Pointer BlurFilterSchedule::NextBlurredImage(void)
{
    if (this->blurAccessed == true && (currentLevel - 1 >= 0))
    {
        //move to the next level, if possible;
        this->currentLevel--;
    }

    return this->CurrentBlurredImage();
}

void BlurFilterSchedule::resetSchedule(void)
{
    this->currentLevel = this->numberOfLevels;
    if (this->numberOfLevels > 1)
    {
        this->stepSize = (this->maxDeviation - this->minDeviation) / (this->numberOfLevels - 1);
    }
    else
    {
        //There is only one level, so step size is irrelevant.
        this->stepSize = 0;
    }

    this->blurAccessed = false;
}
