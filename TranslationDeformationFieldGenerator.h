#pragma once

#include <string>

#include "itkImageRegionIterator.h"
#include "itkImageSource.h"
#include "itkSmartPointer.h"

/**
 * Generates a set of deformation fields consisting of a region that is translated.
 * Deformation
 * fields are images with vector pixel types. The translation deformation
 * field is specified by a starting region, maximum translation, and number
 * of output fields. This ImageSource generates a set of vector fields in
 * which each pixel within the TranslationRegion contains a scaled version
 * of MaxTranslation. For the ith output field, the scaled version is given
 * by MaxTranslation * (i / (N - 1)), where N is the number of outputs.
 */
template < class TDeformationField >
class TranslationDeformationFieldGenerator :
    public itk::ImageSource< TDeformationField >
{
public:
    typedef TranslationDeformationFieldGenerator Self;
    typedef itk::ImageSource< TDeformationField > Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    itkNewMacro(Self);
    itkTypeMacro(TranslationDeformationFieldGenerator, ImageSource);

    // Convenient typedefs
    typedef TDeformationField DeformationType;
    typedef typename DeformationType::PointType PointType;
    typedef typename DeformationType::RegionType RegionType;
    typedef itk::ImageRegionIterator<DeformationType> IteratorType;
    itkStaticConstMacro(ImageDimension, unsigned int, DeformationType::ImageDimension);


    itkGetMacro(MaximumTranslation, PointType);
    itkSetMacro(MaximumTranslation, PointType);

    itkGetMacro(TranslationRegion, RegionType);
    itkSetMacro(TranslationRegion, RegionType);

    itkGetMacro(ImageRegion, RegionType);
    itkSetMacro(ImageRegion, RegionType);

    void SetNumberOfOutputs(unsigned int num)
    {
        Superclass::SetNumberOfOutputs(num);
    }

    unsigned int GetNumberOfOutputs()
    {
        return Superclass::GetNumberOfOutputs();
    }

protected:
    TranslationDeformationFieldGenerator(){}
    virtual ~TranslationDeformationFieldGenerator(){}

    virtual void GenerateData();

private:
    TranslationDeformationFieldGenerator(const Self&);  // not implemented
    void operator=(const Self&);     // not implemented

    PointType m_MaximumTranslation;
    RegionType m_TranslationRegion;
    RegionType m_ImageRegion;
};

/** Implementation **/

template < class TDeformationField >
void TranslationDeformationFieldGenerator< TDeformationField >
::GenerateData()
{
    std::cout << "TranslationDeformationFieldGenerator::GenerateData()..." << std::endl;

    // We do not allocate the output...we create it

    // scale of each translation step
    double scale = 1.0 / (Superclass::GetNumberOfOutputs() - 1);

    for (unsigned int i = 0; i < Superclass::GetNumberOfOutputs(); i++)
    {
        // create an image of the right size
        std::cout << "\tImage[" << i << "]: (" << 
            this->GetImageRegion().GetSize()[0] << ", " << 
            this->GetImageRegion().GetSize()[1] << ")" << std::endl;

        typename DeformationType::Pointer image = DeformationType::New();
        image->SetRegions(this->GetImageRegion());
        image->Allocate();

        // create the pixel to fill in the translation region
        std::cout << "\tTranslation: (";
        typename DeformationType::PixelType pixel;
        typename DeformationType::PixelType zero;
        for (int d = 0; d < DeformationType::PixelType::GetVectorDimension(); d++)
        {
            pixel[d] = i * scale * this->GetMaximumTranslation()[d];
            std::cout << " " << pixel[d];
            zero[d] = 0;
        }
        std::cout << " )" << std::endl;

        std::cout << "\tFilling image with zeros..." << std::endl;
        // create an iterator for the whole image
        IteratorType itAll(image, image->GetLargestPossibleRegion());
        for (itAll.GoToBegin(); !itAll.IsAtEnd(); ++itAll)
        {
            itAll.Set(zero);
        }

        // fill in the translation region
        std::cout << "\tFilling region: (" << 
            this->GetTranslationRegion().GetSize()[0] << ", " <<
            this->GetTranslationRegion().GetSize()[1] << ")" << std::endl;

        // create an iterator for the translation region
        IteratorType it(image, this->GetTranslationRegion());

        for (it.GoToBegin(); !it.IsAtEnd(); ++it)
        {
            it.Set(pixel);
        }

        // graft generated field to output
        std::cout << "\tSetting output " << i << std::endl;
        Superclass::SetNthOutput(i, image);
    }
}

