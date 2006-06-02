#pragma once

#include "itkNumericTraits.h"
#include "itkStatisticsImageFilter.h"
#include "itkUnaryFunctorImageFilter.h"

/**
 * AttenuationFunctor is a functor that returns the ratio of a given input datum
 * to a base value.
 */
template <class TInput, class TOutput>
class AttenuationFunctor
{
public:
    typedef TInput InputType;
    typedef typename itk::NumericTraits< TInput >::RealType RealType;

    AttenuationFunctor() : m_BaseValue(1) {}
    ~AttenuationFunctor() {};

    inline TOutput operator()(const TInput& data)
    {
        return static_cast< TOutput > (data / this->GetBaseValue());
    }

    void SetBaseValue(RealType value)
    { 
        if (value != 0)
            this->m_BaseValue = value;
    }

    RealType GetBaseValue()
    {
        return this->m_BaseValue;
    }

private:
    RealType m_BaseValue;
};

enum BaseValueStyle { Maximum, Mean, Minimum };

/**
 * AttenuationCalibrationFilter produces an attenuation map from a single image. An attenuation
 * map describes by what factor each image location differs from a base value. The base value is
 * typically a metric computed from the entire image, e.g. maximum intensity or mean intensity.
 * The base value metric is controlled by Get/SetBaseValueStyle().
 */
template <class TInputImage, class TOutputImage>
class AttenuationCalibrationFilter :
    public itk::UnaryFunctorImageFilter< TInputImage, TOutputImage, 
        AttenuationFunctor< typename TInputImage::PixelType, typename TOutputImage::PixelType > >
{
public:
    // Standard itk typedefs
    typedef AttenuationCalibrationFilter Self;
    typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage, 
            AttenuationFunctor< typename TInputImage::PixelType, typename TOutputImage::PixelType > > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;

    // Handy typedefs
    typedef TInputImage InputImageType;
    typedef TOutputImage OutputImageType;
    typedef itk::StatisticsImageFilter< InputImageType > StatsType;

    itkNewMacro(Self);
    itkTypeMacro(AttenuationCalibrationFilter, UnaryFunctorImageFilter);

    /** Set the input image. */
    void SetInput(const TInputImage* input)
    {
        Superclass::SetNthInput(0, const_cast<TInputImage*> (input));
    }

    /** Get a reference to the input image */
    TInputImage* GetInput()
    {
        return const_cast<TInputImage*> (Superclass::GetInput(0));
    }

    itkGetMacro(Style, BaseValueStyle);
    itkSetMacro(Style, BaseValueStyle);

protected:
    AttenuationCalibrationFilter() : m_Style(Maximum)
    {
        this->stats = StatsType::New();
    }
    virtual ~AttenuationCalibrationFilter(){}

    /**
     * On update, we first compute the base value to use for the AttenuationFunctor.
     */
    virtual void GenerateData();

private:
    AttenuationCalibrationFilter(const Self&);  // not implemented
    void operator=(const Self&); // not implemented

    typename StatsType::Pointer stats;
    BaseValueStyle m_Style;
};

/** Implementation **/

template <class TInputImage, class TOutputImage>
void AttenuationCalibrationFilter<TInputImage, TOutputImage>
::GenerateData()
{
    // Update the image statistics
    std::cout << "Updating calibration image statistics..." << std::endl;
    this->stats->SetInput(this->GetInput());
    this->stats->Update();

    // Set the attenuation base value, based on the base value style.
    switch (this->m_Style)
    {
    case Mean:
        this->GetFunctor().SetBaseValue(this->stats->GetMean());
        break;
    case Minimum:
        this->GetFunctor().SetBaseValue(this->stats->GetMinimum());
        break;
    default: // Maximum
        this->GetFunctor().SetBaseValue(this->stats->GetMaximum());
        break;
    }

    std::cout << "Base value: " << this->GetFunctor().GetBaseValue() << std::endl;;

    // Resume normal update.
    Superclass::GenerateData();
}
