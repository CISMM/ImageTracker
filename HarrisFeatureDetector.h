#pragma once

#include "itkPointSet.h"
#include "itkTernaryFunctorImageFilter.h"

#include "CommonTypes.h"
#include "Feature.h"

class HarrisFeatureDetector
{
public:
    typedef CommonTypes::InternalImageType ImageType;
    typedef itk::PointSet<Feature, 2> PointSetType;

    HarrisFeatureDetector(int maxCount = 1000, double sigma = 0.7)
    {
        this->SetMaxCount(maxCount);
        this->SetSigma(sigma);
    }

    ~HarrisFeatureDetector(void){}

    PointSetType::Pointer findFeatures(ImageType::Pointer image);
    ImageType::Pointer harrisWeightImage(ImageType::Pointer image);

    int GetMaxCount() { return this->maxCount; }
    double GetSigma() { return this->sigma; }
    void SetMaxCount(int maxCount) { this->maxCount = maxCount; }
    void SetSigma(double sigma) { this->sigma = sigma; }

private:
    int maxCount;
    double sigma;
    PointSetType::Pointer findMaxima(ImageType::Pointer wtImage);
};

namespace Function
{
    /**
     * This functor computes Harris corner detector weighting values
     * for three input image pixels.  The class is templated over the
     * types of input and output pixels.
     */
    template <class TInput1, class TInput2, class TInput3, class TOutput>
    class HarrisWeight
    {
    public:
        HarrisWeight()
        {
            this->weight = 0.04;
        }
        ~HarrisWeight(){}

        /**
         * Overloads () operator.  Computes the Harris weighting function:
         * R = (Ixx*Iyy - Ixy^2) - c * (Ixx + Iyy)^2
         */
        inline TOutput operator()(const TInput1& Pxx, const TInput2& Pyy, const TInput3& Pxy)
        {
            TOutput sumPxxPyy = (TOutput) Pxx + Pyy;
            return (TOutput) 
                (Pxx * Pyy - Pxy * Pxy) - 
                (this->weight * sumPxxPyy * sumPxxPyy);
        }
    private:
        // Weight for (Ixx + Iyy)^2 term.
        double weight;
    };
}

template <class TInputImage1, class TInputImage2, 
          class TInputImage3, class TOutputImage>
class HarrisWeightImageFilter : public itk::TernaryFunctorImageFilter<
                                        TInputImage1, TInputImage2, 
                                        TInputImage1, TOutputImage,
                                        Function::HarrisWeight<
                                            typename TInputImage1::PixelType,
                                            typename TInputImage2::PixelType,
                                            typename TInputImage3::PixelType,
                                            typename TOutputImage::PixelType> >
{
public:
    typedef HarrisWeightImageFilter Self;
    typedef itk::TernaryFunctorImageFilter<
        TInputImage1, TInputImage2, 
        TInputImage1, TOutputImage,
        Function::HarrisWeight<
            typename TInputImage1::PixelType,
            typename TInputImage2::PixelType,
            typename TInputImage3::PixelType,
            typename TOutputImage::PixelType> > Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    itkNewMacro(Self);

protected:
    HarrisWeightImageFilter() {};
    virtual ~HarrisWeightImageFilter() {};

private:
    HarrisWeightImageFilter(const Self&); // not implemented
    void operator=(const Self&); // not implemented
};
